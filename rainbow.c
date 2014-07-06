#include <stdio.h>

main()
{
	int r,g,b,red,green,blue,state;
	char command[40],tmp[5];
	r=0;
	g=0;
	b=0;
	red=1;
	green=0;
	blue=0;
	state = 1;
	while(1)
	{
		switch(state)
		{
			case (1):
				if (red == 1)
				{
					if (r > 100) 
					{	
						r = r + 30;
					}
					if (r > 10) 
					{
						r = r+10;
					}
					r = r + 1;
					if ( r > 255)
					{
						r = 255;
						red = 2;
					}
				} else {
					if (r > 100) r = r - 30;
					if (r > 20) r = r - 10;
					r = r - 1;
					if (r < 1)
					{
						r = 0;
						red = 1;
						state = 2;
						green = 1;
					}
				}
				break;
			case(2):
				if (green == 1)
				{
					g = g + 1;
					if (g > 10) g = g+10;
					if (g > 100) g = g + 30;
					if (g > 254)
					{
						green = 2;
						g = 255;
					}
				} else {
					if (g > 100) g = g - 30;
					if (g > 20) g = g - 10;
					g = g - 1;
					if (g < 1 )
					{
						g = 0;
						green = 1;
						state = 3;
						blue = 1;
					}
				}
				break;
			case(3):
				if (blue == 1 )
				{
					b = b + 1;
					if (b > 10) b = b+10;
					if (b > 100) b = b + 30;
					if (b > 254)
					{
						blue = 2;
						b = 255;
					}
				} else {
					if (b > 100) b = b - 30;
					if (b > 20) b = b - 10;
					b = b - 1;
					if (b < 1)
					{
						blue = 1;
						state = 1;
						b = 0;
						red = 1;
					}
				}	
				break;
			default:
				r = 0;
				g = 0;
				b = 0;
				state = 1;
				break;
		}
		//if (r > 255) r = 255;
		//if (r < 0) r = 0;
		//if (g > 255) g = 255;
		//if (g < 0) g = 0;
		//if (b > 255) b = 255;
		//if (b < 0) b = 0;
		strcpy(command,"./led ");
		sprintf(tmp," %d ",r);
		strcat(command,tmp);
		sprintf(tmp," %d ",g);
		strcat(command,tmp);
		sprintf(tmp," %d ",b);
		strcat(command,tmp);
		system(command);
		printf("state = %d\t%s\n",state,command);
		sleep(5);
	}
}
