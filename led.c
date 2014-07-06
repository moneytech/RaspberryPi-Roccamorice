#include <stdio.h>


 main(int argc, char **argv)
{
        unsigned int r,g,b,r1,r2,g1,g2,b1,b2,enable;
        char command[120],tmp[5];
        if (argc != 4)
        {
                printf("Error, need r g b values\n");
                //exit(0);
        } else {
                //printf("Good job!\n");
                //printf("first (red) value %s\n",argv[1]);
                //printf("second (green) value %s\n",argv[2]);
                //printf("third (blue) value %s\n",argv[3]);
                sscanf(argv[1],"%d",&r);
                sscanf(argv[2],"%d",&g);
                sscanf(argv[3],"%d",&b);
                //printf("r = %d\tg = %d\tb = %d\n",r,g,b);

                //add 2 and 3 to set red register
                r1 = (r & 0xf0) + 2;
                r1 = ~r1;
                r1 = r1 & 0xff;
                r2 = ((0xf & r) << 4) + 3;
                r2 = ~r2;
                r2 = r2 & 0xff;

                //add 0 adn 1 to set green register
                g1 = (g & 0xf0) + 0;
                g1 = ~g1;
                g1 = g1 & 0xff;
                g2= ((g & 0xf) << 4) + 1;
                g2 = ~g2;
                g2 = g2 & 0xff;
                //finally add 4 and 5 to set green register
                b1 = (b & 0xf0) + 4;
                b1 = ~b1;
                b1 = b1 & 0xff;
                b2 = ((b & 0xf) << 4) + 5;
                b2 = ~b2;
                b2 = b2 & 0xff;
                enable = 137;
                //printf("r1 = %d\tr2= %d\n",r1,r2);
                //printf("g1 = %d\tg2= %d\n",g1,g2);
                //printf("b1 = %d\tb2= %d\n",b1,b2);

                //set the strobe
                strcpy(command,"echo \"1");
                sprintf(tmp,"%d",1);
                strcat(command,tmp);
                strcat(command,"\"");
                //you will need to change the path and 29.xxxx address for your system...
                strcat(command," > /mnt/1wire/29.F6C511000000/strobe");
                system (command);
                //printf("strobe command: %s\n",command);

                //now simply form the string command  to pass to system(command)
                strcpy(command,"echo \"");
                sprintf(tmp,"%d",r1);
                strcat(command,tmp);
                strcat(command,"\"");
                strcat(command," > /mnt/1wire/29.F6C511000000/PIO.BYTE");
                system (command);
                //printf("r1 command: %s\n",command);

                strcpy(command,"echo \"");
                sprintf(tmp,"%d",r2);
                strcat(command,tmp);
                strcat(command,"\"");
                strcat(command," > /mnt/1wire/29.F6C511000000/PIO.BYTE");
                system (command);
                //printf("r2 command: %s\n",command);

                strcpy(command,"echo \"");
                sprintf(tmp,"%d",g1);
                strcat(command,tmp);
                strcat(command,"\"");
                strcat(command," > /mnt/1wire/29.F6C511000000/PIO.BYTE");
                system (command);
                //printf("g1 command: %s\n",command);

                strcpy(command,"echo \"");
                sprintf(tmp,"%d",g2);
                strcat(command,tmp);
                strcat(command,"\"");
                strcat(command," > /mnt/1wire/29.F6C511000000/PIO.BYTE");
                system (command);
                //printf("g2 command: %s\n",command);

                strcpy(command,"echo \"");
                sprintf(tmp,"%d",b1);
                strcat(command,tmp);
                strcat(command,"\"");
                strcat(command," > /mnt/1wire/29.F6C511000000/PIO.BYTE");
                system (command);
                //printf("b1 command: %s\n",command);

                strcpy(command,"echo \"");
                sprintf(tmp,"%d",b2);
                strcat(command,tmp);
                strcat(command,"\"");
                strcat(command," > /mnt/1wire/29.F6C511000000/PIO.BYTE");
                system (command);
                //printf("b2 command: %s\n",command);

                strcpy(command,"echo \"");
                sprintf(tmp,"%d",enable);
                strcat(command,tmp);
                strcat(command,"\"");
                strcat(command," > /mnt/1wire/29.F6C511000000/PIO.BYTE");
                system (command);
                //printf("enable command: %s\n",command);
        }
}


