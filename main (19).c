#include <kipr/botball.h>

int main()
{
    int burning_position = 0;
    int botguy_position = 0;
    int mayor_position = 0;
    int tracking_color = 0;
    void claw(int position){
        set_servo_position(2, position);
    }
    void lift(int start, int end){
        int position = start;
        int range = end - start;
        while (position != end){
            set_servo_position(1, position);
            set_servo_position(0, (-1 * position) + 2047);
            position += range/50;
            msleep(15);
        }
    }

    void line_follow(int time, int speed){
        int time_end = time;
        time = 0;
        int adjust,adjust2 = 0;
        while(time < time_end){
            adjust = ((analog(1) - 1850)/10);
            adjust2 = (adjust*adjust*adjust)/100000;
            create_drive_direct(-speed + adjust2+0, -speed - adjust2);
            msleep(10);
            time += 10;
        }
    }


    void Wait_until_light(int threshold){
        int light_on = 0;
        while (light_on != 1){
            if(analog(0) < threshold || analog(2) == 1){
                light_on = 1;
            }

        }
    }
    void accelerate(int speed_start, int speed_end,int rate){
        int amount = (speed_start - speed_end)*-1;
        int current_speed = speed_start;
        while(current_speed != speed_end){
            create_drive_direct(current_speed, current_speed);
            msleep(rate);
            current_speed += amount/50;
        }
    }
    void track_object(int speed, int duration, int chnl)
    {
        printf("tracking channel %d",chnl);
        camera_open_at_res(LOW_RES);
        int duration2 = 0;
        int adjust2 = 0;
        while (duration2 < duration){
            msleep(10);
            duration2 += 10;
            camera_update();
            if (get_object_count(chnl) == 0){
                printf("no objects.\n");
            }
            else{
                adjust2 = get_object_center_x(chnl,0) - 80;
                create_drive_direct(speed+adjust2,speed-adjust2);
                printf("%d.\n",adjust2);
            }
        }    
        create_drive_direct(0,0);

    }
    void edge_follow(int duration){
        int time = 0;
        int adjust, adjust2 = 0;
        while (time < duration){
            printf("%d",analog(1));
            adjust = ((analog(1) - 1850)/10);
            adjust2 = (adjust*adjust*adjust)/100000;
            create_drive_direct(adjust2, adjust2);
            msleep(10);
            time += 10;	
        }

    }
    int check_for_object(int chnl, int area){
        int repititions = 0;
        int tolerance = 0;
        for(repititions=0; repititions != 30; repititions++){
            camera_update();
            if(tolerance > 15){
                return 1;
                repititions = 20;
            }
            if(get_object_count(chnl) == 0){
                printf("no objects in sight\n");
            }
            else{
                printf("object found  ");
                if(get_object_area(chnl,0) > area){
                    printf("object greater than specified area %d\n",tolerance);
                    tolerance++;
                }
                else{
                    printf("object not greater than specified area %d\n", tolerance);
                }
            }
        }
        if(tolerance <= 15){
            return 0;
        }
    }
    void scan(){
        //open camera

        camera_open_at_res(LOW_RES);

        create_drive_direct(100,-100);
        msleep(750);
        create_drive_direct(0,0);
        burning_position = check_for_object(1,100);

        if(burning_position == 0){
            create_drive_direct(100,-100);
            msleep(250);
            create_drive_direct(0,0);
            burning_position = check_for_object(1,60) * 2; 

        }

        if(burning_position == 0){
            create_drive_direct(100,-100);
            msleep(550);
            create_drive_direct(0,0);
            burning_position = check_for_object(1,60) * 3; 

        }
        if(burning_position == 1){
            create_drive_direct(50,-50);
            msleep(500);
            create_drive_direct(0,0);
            lift(400,1400);
            if(check_for_object(3,70) == 1){
                mayor_position = 2;
                botguy_position = 3;
            }
            else{
                mayor_position = 3;
                botguy_position = 2;
            }
        }
        if(burning_position == 2){
            create_drive_direct(100,-100);
            msleep(425);
            create_drive_direct(0,0);
            lift(400,1000);
            if(check_for_object(3,70) == 1){
                mayor_position = 3;
                botguy_position = 1;
            }
            else{
                mayor_position = 1;
                botguy_position = 3;
            }
        }
        if(burning_position == 3){
            create_drive_direct(-50,50);
            msleep(350);
            create_drive_direct(0,0);
            lift(400,1400);
            if(check_for_object(3,70) == 1){
                mayor_position = 2;
                botguy_position = 1;
            }
            else{
                mayor_position = 1;
                botguy_position = 2;
            }
        }
        printf("burning_position=%d\n",burning_position);
        printf("mayor_position=%d\n",mayor_position);
        printf("botguy_position=%d\n",botguy_position);


    }




    create_connect();	
    enable_servos();
    camera_open_at_res(LOW_RES);
    claw(0);
    set_servo_position(0,1647);
    set_servo_position(1,400);
    Wait_until_light(100);
    claw(900);
    scan();
    if(burning_position == 3 ||burning_position == 1){
        if(mayor_position == 2){
            tracking_color = 3;
        }
        else{
            tracking_color = 2;
        }
        track_object(0,100,tracking_color);
        lift(1400,1200);
        track_object(-150,700,tracking_color);
        lift(1200,1100);
        accelerate(0,-100,10);
        accelerate(-100,0,10);
        if(mayor_position == 2){
            lift(1100,1050);
            claw(0);
            lift(1000,1400);
        }
        else{
            lift(1100,1050);
            accelerate(0,-100,10);
            accelerate(-100,0,10);
            claw(0);
            lift(1000,1400);
        }
        accelerate(0,300,10);
        msleep(700);
        accelerate(300,0,10);
        create_drive_direct(-200,200);
        msleep(1500);
        create_drive_direct(0,0);
        lift(1400,0);
        claw(800);
        lift(0,900);
        if(mayor_position == 2){
            tracking_color = 2;
        }
        else{
            tracking_color = 3;
        }
        if(burning_position == 1){
            create_drive_direct(-200,200);
            msleep(1800);
            create_drive_direct(0,0);
            track_object(0,100,tracking_color);
            track_object(-150,400,tracking_color);
            accelerate(0,-100,10);
            accelerate(-100,0,10);
            lift(900,700);
            claw(0);
            lift(700,1400);
            accelerate(0,300,20);
            msleep(800);
            accelerate(300,0,20);
            create_drive_direct(-200,200);
            msleep(1700);
            create_drive_direct(0,0);
            lift(1400,200);
            claw(900);
            lift(200,400);
            create_drive_direct(-100,100);
            msleep(500);
            create_drive_direct(0,0);
            accelerate(0,300,10);
            msleep(400);
            accelerate(300,0,10);
            create_drive_direct(-100,100);
            msleep(1700);
            create_drive_direct(0,0);
            lift(300,0);
            line_follow(2000,-100);
            accelerate(0,300,10);
            msleep(700);
            accelerate(300,0,10);
        }
        else{
            create_drive_direct(100,-100);
            msleep(2000);
            create_drive_direct(0,0);
            track_object(0,100,tracking_color);
            track_object(-150,400,tracking_color);
            accelerate(0,-100,20);
            accelerate(-100,0,20);
            lift(900,800);

            claw(0);
            msleep(500);
            lift(800,1400);
            accelerate(0,300,20);
            msleep(800);
            accelerate(300,0,20);
            create_drive_direct(-200,200);
            msleep(1250);
            create_drive_direct(0,0);
            lift(1400,300);
            claw(900);
            
            create_drive_direct(-100,100);
            msleep(300);
            create_drive_direct(0,0);
            accelerate(0,300,10);
            msleep(400);
            accelerate(200,0,20);
            create_drive_direct(-100,100);
            msleep(1700);
            create_drive_direct(0,0);
            lift(300,0);
            line_follow(2000,-100);
            accelerate(0,300,10);
            msleep(1000);
            accelerate(300,0,20);
        }

    }
    else{
        if(mayor_position == 3){
            tracking_color = 3;
        }
        else{
            tracking_color = 2;
        }
        track_object(0,100,tracking_color);
        track_object(-150,500,tracking_color);
        accelerate(0,-150,10);
        msleep(900);

        accelerate(-150,0,10);
        lift(900,750);
        claw(0);
        msleep(1000);
        lift(750,1400);
        accelerate(0,300,10);
        msleep(700);
        accelerate(300,0,10);
        create_drive_direct(-100,100);
        msleep(3000);
        create_drive_direct(0,0);
        lift(1400,200);
        claw(900);
        if(mayor_position == 3){
            tracking_color = 2;
        }
        else{
            tracking_color = 3;
        }
        lift(200,900);
        create_drive_direct(100,-100);
        msleep(1200);
        create_drive_direct(0,0);
        accelerate(0,-200,20);
        msleep(500);
        accelerate(-200,0,20);
        create_drive_direct(100,-100);
        msleep(1200);
        create_drive_direct(0,0);
        track_object(0,100,tracking_color);
        track_object(-150,350,tracking_color);
        accelerate(0,-100,20);
        msleep(500);
        accelerate(-100,0,20);
        lift(900,750);
        claw(0);
        msleep(300);
        lift(700,1400);
        accelerate(0,300,10);
        msleep(800);
        accelerate(300,0,10);
        create_drive_direct(-200,200);
        msleep(1800);
        create_drive_direct(0,0);
        lift(1400,200);
        claw(900);
        accelerate(0,300,10);
        msleep(200);
        accelerate(200,0,10);
        create_drive_direct(-100,100);
        msleep(1700);
        create_drive_direct(0,0);
        lift(300,0);
        line_follow(2000,-100);
        accelerate(0,200,10);
        msleep(100);
        accelerate(200,0,10);

    }










    return 0;

}
