package com.example.oscarjosueulatealpizar.fingerme;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class MainActivity extends AppCompatActivity {

    Spinner spinner;
    static final String[] paths = {"1:1", "2:2", "4:4"};

    Button pinIt_btn;
    Button ok_btn;

    TextView pin_label;
    TextView pin_input;

    List<Button> button_list = new ArrayList<>();

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        pinIt_btn = (Button)findViewById(R.id.pinIt_btn);
        ok_btn = (Button)findViewById(R.id.ok_btn);

        pin_label = (TextView)findViewById(R.id.pin_label);
        pin_input = (TextView)findViewById(R.id.pin_input);

        button_list.add((Button)findViewById(R.id.btn0));
        button_list.add((Button)findViewById(R.id.btn1));
        button_list.add((Button)findViewById(R.id.btn2));
        button_list.add((Button)findViewById(R.id.btn3));
        button_list.add((Button)findViewById(R.id.btn4));
        button_list.add((Button)findViewById(R.id.btn5));
        button_list.add((Button)findViewById(R.id.btn6));
        button_list.add((Button)findViewById(R.id.btn7));
        button_list.add((Button)findViewById(R.id.btn8));
        button_list.add((Button)findViewById(R.id.btn9));

        spinner = (Spinner)findViewById(R.id.size_spinner);

        ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this,
                android.R.layout.simple_spinner_item,paths);

        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner.setAdapter(adapter);
        spinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> parent, View view, int position, long id) {
                switch (position) {
                    case 0:
                        set_buttons_size(1);
                        break;
                    case 1:
                        set_buttons_size(2);
                        break;
                    case 2:
                        set_buttons_size(4);
                        break;
                }
            }

            @Override
            public void onNothingSelected(AdapterView<?> parent) {
                set_buttons_size(4);
            }
        });

        ok_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String str1 = pin_label.getText().toString();
                String str2 = pin_input.getText().toString();
                if (str1.equals(str2)){
                    pin_input.setText("");
                    pin_label.setAllCaps(true);
                    pin_label.setText("VERY GOOD!!");
                }
                else {
                    pin_input.setText("");
                    pin_label.setAllCaps(true);
                    pin_label.setText("TRY AGAIN :(");
                }

            }
        });

        pinIt_btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int random = randomPin();
                String str = String.valueOf(random);
                System.out.println(str);
                pin_label.setText(str);
            }
        });

        for (int i = 0; i < button_list.size(); i++){
            final int finalI = i;
            button_list.get(i).setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    pin_input.setText(addNumber(finalI));
                }
            });
        }

    }

    public void set_buttons_size(int cm_size){

        DisplayMetrics dm = new DisplayMetrics();
        int widthPixels = 1280;
        int heightPixels = 800;

        double in_size = cm_size + 0.2;
        double px_x_size = in_size * widthPixels / 22;
        double px_y_size = in_size * heightPixels / 13.7;

        System.out.println("px_x_size: " + px_x_size);

        ViewGroup.LayoutParams params;
        params = ok_btn.getLayoutParams();
        params.width = (int)px_x_size;
        params.height = (int) px_y_size;
        ok_btn.setLayoutParams(params);

        params = pinIt_btn.getLayoutParams();
        params.width = (int)px_x_size;
        params.height = (int) px_y_size;
        pinIt_btn.setLayoutParams(params);

        for (int i = 0; i < button_list.size(); i++){
            params = button_list.get(i).getLayoutParams();
            params.width = (int)px_x_size;
            params.height = (int) px_y_size;
            button_list.get(i).setLayoutParams(params);
        }

        // Also sets the labels and dropdown size


    }


    public String addNumber(int i){
        String str = pin_input.getText().toString();
        if (str.length() == 0){
            str = String.valueOf(i);
            return str;
        }
        String number = String.valueOf(i);
        str = str.concat(number);
        return str;
    }



    /*
     * Generates a random number between 100000 and 999999.
     */
    public int randomPin(){
        Random aRandom = new Random();

        int aStart = 100000; //So the PIN never starts with zero
        int aEnd = 1000000;  //So the PIN tops 6 figures

        //get the range, casting to long to avoid overflow problems
        long range = (long)aEnd - (long)aStart + 1;
        long fraction = (long)(range * aRandom.nextDouble());
        int randomNumber =  (int)(fraction + aStart);
        return randomNumber;
    }
}