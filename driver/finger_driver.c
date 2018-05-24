#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kref.h>
#include <linux/usb.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#define USB_ROBOTICFINGER_VENDOR_ID	0x2341	     // The vendor ID
#define USB_ROBOTICFINGER_PRODUCT_ID 0x0043	     // The Device ID
#define USB_ROBOTICFINGER_MINOR_BASE 192         // Minor Base 

MODULE_LICENSE("GPL");                       // The license type
MODULE_AUTHOR("JOF");                       // The author
MODULE_DESCRIPTION("Arduino driver to control a robotic finger hardware");  // The description
MODULE_VERSION("0.7.2");                       // The driver version

static DEFINE_MUTEX(fs_mutex); // Defining a mutex


static struct usb_device_id robotic_finger_table [] = {
	{ USB_DEVICE(USB_ROBOTICFINGER_VENDOR_ID, USB_ROBOTICFINGER_PRODUCT_ID) },
	{ }
};
MODULE_DEVICE_TABLE (usb, robotic_finger_table);

struct usb_robotic_finger {
	struct usb_device *	udev;			// the usb device
	struct usb_interface *	interface;		// the interface for this device
	unsigned char *		bulk_in_buffer;		// the buffer to receive data
	size_t			bulk_in_size;		// the size of the receive buffer
	__u8			bulk_in_endpointAddr;	// the address of the bulk in endpoint
	__u8			bulk_out_endpointAddr;	// the address of the bulk out endpoint
	struct kref		kref;
};


#define to_robotic_finger_dev(d) container_of(d, struct usb_robotic_finger, kref)

static struct usb_driver robotic_finger_driver;

/**
* Free the bulk buffer
*/
static void robotic_finger_delete(struct kref *kref )  {
	struct usb_robotic_finger *dev = to_robotic_finger_dev(kref);

	usb_put_dev(dev->udev);
	kfree (dev->bulk_in_buffer);
	kfree (dev);
}

/**
 *  The device open function that is called each time the device is opened
 */
static int robotic_finger_open(struct inode *inode, struct file *file )  {
	struct usb_robotic_finger *dev;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	subminor = iminor(inode);

	interface = usb_find_interface(&robotic_finger_driver, subminor);
	if (!interface) {
		printk(KERN_INFO "finger_driver: %s - error, can't find device for minor %d",
		     __FUNCTION__, subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		retval = -ENODEV;
		goto exit;
	}

	kref_get(&dev->kref);
	file->private_data = dev;

  exit:
  	return retval;
}

/**
 *  The device release function that is called whenever the device is closed/released by
 *  the userspace program
 */
static int robotic_finger_release(struct inode *inode, struct file *file )  {
	struct usb_robotic_finger *dev;

	dev = (struct usb_robotic_finger *) file->private_data;
	if (dev == NULL)
		return -ENODEV;

	kref_put(&dev->kref, robotic_finger_delete);
	return 0;
}

/**
 *  This function is called when the device is being read from user space
 */
static ssize_t robotic_finger_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos) {
	int count_int = (int) count;
	struct usb_robotic_finger *dev;
	int retval = 0;

	dev = (struct usb_robotic_finger*) file->private_data;

	retval = usb_bulk_msg(dev->udev,
			      usb_rcvbulkpipe(dev->udev, dev->bulk_in_endpointAddr),
			      dev->bulk_in_buffer,
			      min(dev->bulk_in_size, count),
			      &count_int, HZ*10);

	if (!retval) {
		if (copy_to_user(buffer, dev->bulk_in_buffer, count))
			retval = -EFAULT;
		else
			retval = count;
	}

	printk(KERN_INFO "finger_driver: Error reading retval=%d",retval);
	return retval;
}


/**
* bulk callback
*/
static void robotic_finger_write_bulk_callback(struct urb *urb )  {
	if (urb->status &&
	    !(urb->status == -ENOENT ||
	      urb->status == -ECONNRESET ||
	      urb->status == -ESHUTDOWN)) {
		printk(KERN_INFO "finger_driver: %s - nonzero write bulk status received: %d",
		    __FUNCTION__, urb->status);
	}

	 usb_free_coherent(urb->dev, urb->transfer_buffer_length,
			urb->transfer_buffer, urb->transfer_dma);
}

/**
* This function is called data is received from the user space to the device
*/
static ssize_t robotic_finger_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *ppos) {

	struct usb_robotic_finger *dev;
	int retval = 0;
	struct urb *urb = NULL;
	char *buf = NULL;

	dev = (struct usb_robotic_finger *) file->private_data;

	if (count == 0)
		goto exit;

	urb = usb_alloc_urb(0, GFP_KERNEL);
	if (!urb) {
		retval = -ENOMEM;
		goto error;
	}

	buf =  usb_alloc_coherent(dev->udev, count, GFP_KERNEL, &urb->transfer_dma);
	if (!buf) {
		retval = -ENOMEM;
		goto error;
	}
	if (copy_from_user(buf, user_buffer, count)) {
		retval = -EFAULT;
		goto error;
	}

	usb_fill_bulk_urb(urb, dev->udev, usb_sndbulkpipe(dev->udev, dev->bulk_out_endpointAddr),
						buf, count, robotic_finger_write_bulk_callback, dev);
	urb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;

	retval = usb_submit_urb(urb, GFP_KERNEL);
	if (retval) {
		printk(KERN_INFO "finger_driver: %s - failed submitting write usb, error %d", __FUNCTION__, retval);
		goto error;
	}

	printk(KERN_INFO "finger_driver: successful write");
	usb_free_urb(urb);

  exit:
  	return count;

  error:
  	usb_free_coherent(dev->udev, count, buf, urb->transfer_dma);
  	usb_free_urb(urb);
  	kfree(buf);
  	return retval;
  }

/**
 *  Devices are represented as file structure in the kernel.
 */
static struct file_operations robotic_finger_fops = {
	.owner =	THIS_MODULE,
	.read =		robotic_finger_read,
	.write =	robotic_finger_write,
	.open =		robotic_finger_open,
	.release =	robotic_finger_release,
};

/**
* Device operations, minor number boundaries, and file name
*/
static struct usb_class_driver robotic_finger_class = {

	.name = "finger%d",
	.fops = &robotic_finger_fops,
	.minor_base = USB_ROBOTICFINGER_MINOR_BASE,

};

/**
* This function is executed when the device is connected and it is needed to test
*/
static int robotic_finger_probe(struct usb_interface *interface, const struct usb_device_id *id )  {

	struct usb_robotic_finger *dev = NULL;
	struct usb_host_interface *iface_desc;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
	int i;
	int retval = -ENOMEM;

	dev = kmalloc(sizeof(struct usb_robotic_finger), GFP_KERNEL);
	if (dev == NULL) {
		printk(KERN_INFO "finger_driver: Out of memory");
		goto error;
	}
	memset(dev, 0x00, sizeof (*dev));
	kref_init(&dev->kref);

	dev->udev = usb_get_dev(interface_to_usbdev(interface));
	dev->interface = interface;

	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;

		if (!dev->bulk_in_endpointAddr &&
		    (endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			buffer_size = endpoint->wMaxPacketSize;
			dev->bulk_in_size = buffer_size;
			dev->bulk_in_endpointAddr = endpoint->bEndpointAddress;
			dev->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);
			if (!dev->bulk_in_buffer) {
				printk(KERN_INFO "finger_driver: Could not allocate bulk_in_buffer");
				goto error;
			}
		}

		if (!dev->bulk_out_endpointAddr &&!(endpoint->bEndpointAddress & USB_DIR_IN) &&((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)
					== USB_ENDPOINT_XFER_BULK)) {
			dev->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		}
	}
	if (!(dev->bulk_in_endpointAddr && dev->bulk_out_endpointAddr)) {
		printk(KERN_INFO "finger_driver: Could not find both bulk-in and bulk-out endpoints");
		goto error;
	}

	usb_set_intfdata(interface, dev);

	retval = usb_register_dev(interface, &robotic_finger_class);
	if (retval) {
		printk(KERN_INFO "finger_driver: Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	printk(KERN_INFO "finger_driver: device now attached to finger%d", interface->minor);
	return 0;

    error:
  	if (dev)
  		kref_put(&dev->kref, robotic_finger_delete);
  	return retval;
}

/**
* This fucntion describes the actions when the device is disconnected
*/
static void robotic_finger_disconnect(struct usb_interface *interface)  {
	struct usb_robotic_finger *dev;
	int minor = interface->minor;

	mutex_lock(&fs_mutex);

	dev = usb_get_intfdata(interface);
	usb_set_intfdata(interface, NULL);

	usb_deregister_dev(interface, &robotic_finger_class);

	mutex_unlock(&fs_mutex);

	kref_put(&dev->kref, robotic_finger_delete);

	printk(KERN_INFO "finger_driver: #%d now disconnected", minor);
}

static struct usb_driver robotic_finger_driver = {
	.name = "finger_driver",
	.id_table = robotic_finger_table,
	.probe = robotic_finger_probe,
	.disconnect = robotic_finger_disconnect,
};

/**
 *  The device driver initialization function.
 */
static int __init usb_robotic_finger_init(void) {
	int res;

	res = usb_register(&robotic_finger_driver);
	if (res )
		printk(KERN_INFO "finger_driver: usb_register failed. Error number %d", res);

	printk(KERN_INFO "finger_driver: device registered");
	return res;
}

/**
 *  The function do the device driver cleanup
 */
static void __exit usb_robotic_finger_exit(void)  {
	usb_deregister(&robotic_finger_driver);
	printk(KERN_INFO "finger_driver: device deregistered");
}

module_init(usb_robotic_finger_init ); // Initialize the driver
module_exit(usb_robotic_finger_exit ); // Cleanup the driver
