/*** Cal Poly 1995 Micromouse Control and Mapping Code ***/

/*** defined constants ***/

#define LED_Port 0x08

#define Port1A 0x00
#define Port1B 0x01
#define Port1C 0x02
#define Cmd1   0x03

#define Port2A 0x08
#define Port2B 0x09
#define Port2C 0x0A
#define Cmd2   0x0B

#define maxstack 257
#define true 1
#define false 0
#define north 0
#define south 1
#define east 2
#define west 3
#define going_forward 0
#define turning_left 1
#define turning_right 2
#define turning_around 3
#define forw 1
#define back 0
#define forward_full 50                   /**/
#define turn_full 100                      /**/
#define around_full 140                    /**/
#define forward_pulses_a 70               /**/
#define forward_pulses_b 185               /*190*/
#define forward_pulses_c 70               /**/
#define turn_pulses_a 40          /**/
#define turn_pulses_b 77          /*75*/
#define turn_pulses_c 40          /**/
#define around_pulses_a 70        /**/
#define around_pulses_b 174       /*172*/
#define around_pulses_c 70        /**/
#define stop 450                           /**/
#define check_freq 45 
#define check_step 7

/***** prototypes *****/



void Initialize (void);
void Show_Pattern(void);
void Start_Up_Jump(void);
void Rotate(void);
void read_sensors();
void motors();
void motorL();
void motorR();
void leftforward();
void rightforward();
void leftbackward();
void rightbackward();

void create (void);
short int is_empty (void);
void push (void);
void pop (void);
short int stack_top (void);
void wait (void);
short int forward_blocked (void);
short int left_blocked (void);
short int right_blocked (void);
short int back_blocked (void);
short int north_blocked (void);
short int south_blocked (void);
short int east_blocked (void);
short int west_blocked (void);
void check_centering (void);
void accelerate (void);
void decelerate (void);
void turn_left (void);
void turn_right (void);
void go_forward (void);
void turn_around (void);
void search_junction (void);
void go_north (void);
void go_south (void);
void go_east (void);
void go_west (void);
short int map_blocked (void);
void dest_found (void);
void check_dest (void);
void move2 (void);
void move (void);
void backtrack (void);
void motorL (void);
void motorR (void);
void motors (void);
void initialize2 (void);
short int twotothe (void);
short int sensormask (void);
void makemasks (void);
short int twotothe (void);
short int sensormask (void);

/* other prototypes here */


/***** types *****/

struct maptype
{
	short int old;
	short int number;
	short int exits;
	short int typ;
};

struct stack_type
{       
	short int top;
	short int items[maxstack];
};


/***** variables *****/

unsigned char Pattern1, l, forward, right, left;
short int facing, count, i, j, doing_now, x, y, dest_x, dest_y;
short int done_flag, changed, a, b, c, d, e;
short int stack_number, newitem, o, temp_x, temp_y, item1, k, direction;
short int mask, current, coord, item2, t, stack_num, center7;
short int ty, exi, through;
short int start, finish, pulses, delay, word, step_inc, t1, t2, t3;
short int word, checked, l1, l2, l3, l4, l5, l6, r1, r2, r3, r4, r5, r6; 
short int q, nr, nl;

struct maptype map[18][18];
struct stack_type backstack;
struct stack_type newstack;


/***** stack procedures *****/

void create (void)
{       if (stack_number==0) backstack.top=0;
	else if (stack_number==1) newstack.top=0;
}

short int is_empty (void)
{       i=false;
	switch(stack_number)
	{       case 0:
			if (backstack.top==0) i=true; 
			else i=false;
			break;
		case 1:
			if (newstack.top==0) i=true; 
			else i=false;
			break;
		default: ;
	}
	return i;
}

void push (void)
{       switch(stack_number)
	{
		case 0:
			backstack.top++;
			backstack.items[backstack.top]=newitem;
			break;
		case 1:
			newstack.top++;
			newstack.items[newstack.top]=newitem;
			break;
	}
}

void pop (void)
{       switch(stack_number)
	{
		case 0:
			stack_number=0;
			if (is_empty()) ;
			else backstack.top--;
			break;
		case 1:
			stack_number=1;
			if (is_empty()) ;
			else newstack.top--;
			break;
		default: ;
	}
}

short int stack_top (void)
{               switch(stack_number)
		{       
			case 0:
				stack_number=0;
				if (is_empty()) ;
				else  i=backstack.items[backstack.top];
				break;
			case 1:
				stack_number=1;
				if (is_empty()) ;
				else  i=newstack.items[newstack.top];
			default: ;
		}
		return i;
}


/***** delay *****/

void wait (void)
{       for (word=0; word < delay; word++) Show_Pattern();
}


/***** forward/left/right/back blocked routines *****/

short int forward_blocked (void)
{       read_sensors();
	if ((forward & 128) == 128) return true;
	else return false;
}

short int outside_blocked (void)
{       read_sensors();
	if ((forward & 64) == 64) return true;
	else return false;
}

short int left_blocked (void)
{       read_sensors();
	if ((left & 18) == 18) return true;
	else return false;
}

short int right_blocked (void)
{       read_sensors();
	if ((right & 18) == 18) return true;
	else return false;
}

short int back_blocked (void)
{       if ((x==1) & (y==1) & (facing==north)) return true;
	else return false;
}


/***** north/south/east/west blocked routines *****/

short int north_blocked (void)
{       if ((i==1) & (map[x][y+1].old==true)) return true;
	else {switch(facing)
		{       case north: k=forward_blocked();
				break;
			case south: k=back_blocked();
				break;
			case east: k=left_blocked();
				break;
			case west: k=right_blocked();
				break;
			default: ;
		}
		return k;
		  }
}

short int south_blocked (void)
{       if ((i==1) & (map[x][y-1].old==true)) return true;
	else {switch(facing)
		{       case north: k=back_blocked();
				break;
			case south: k=forward_blocked();
				break;
			case east: k=right_blocked();
				break;
			case west: k=left_blocked();
				break;
			default: ;
		}
		return k;
		  }
}

short int east_blocked (void)
{       if ((i==1) & (map[x+1][y].old==true)) return true;
	else {switch(facing)
		{       case north: k=right_blocked();
				break;
			case south: k=left_blocked();
				break;
			case east: k=forward_blocked();
				break;
			case west: k=back_blocked();
				break;
			default: ;
		}
		return k;
		  }
}

short int west_blocked (void)
{  if ((i==1) & (map[x-1][y].old==true)) return true;
	else {switch(facing)
		{       case north: k=left_blocked();
				break;
			case south: k=right_blocked();
				break;
			case east: k=back_blocked();
				break;
			case west: k=forward_blocked();
				break;
			default: ;
		  }
		return k;
		}
}


/*** error correction routines ***/

void makemasks (void)
{       read_sensors();
	d=1; q=sensormask(); l1=(left & q); r1=(right & q);
	d=2; q=sensormask(); l2=(left & q); r2=(right & q);
	d=3; q=sensormask(); l3=(left & q); r3=(right & q);
	d=4; q=sensormask(); l4=(left & q); r4=(right & q);
	d=5; q=sensormask(); l5=(left & q); r5=(right & q);
	d=6; q=sensormask(); l6=(left & q); r6=(right & q);

}       

void check_centering (void)
{       makemasks();
	if ((l6 != 0) | (l3 != 0) | (r6 != 0) | (r3 != 0))
	{       for (i=0; i < check_step; i++)
		{       Pattern1=1;
			Show_Pattern();
			motorR();
			wait();
			nl=0;
			nr++;
		}
	}
	if ((l4 != 0) | (l1 != 0) | (r4 != 0) | (r1 != 0))
	{       for (i=0; i < check_step; i++)
		{       Pattern1=128;
			Show_Pattern();
			motorL();
			wait();
			nr=0;
			nl++;
		}
	}
}                
	


/*** JOE'S ROUTINES ***/

void Initialize(void)
{
/*Initialize the 8255_1 */
/* Port A,B,C = Input Devices */
asm MOV AL,8BH;
asm OUT Cmd1,AL;


/*Initialize the 8255_2 */
/* Port A,B = Output Devices, Port C = Input Device */
asm MOV AL,89H;
asm OUT Cmd2,AL;

Pattern1=0x01; 
}


void Show_Pattern( void )
{
  asm MOV AL,Pattern1;
  asm OUT LED_Port, AL;
}

void Rotate(void)
{
  asm MOV AL,Pattern1;
  asm ROL AL,1;
  asm MOV Pattern1,AL;
}


void read_sensors(void)
{
  asm IN AL,Port1B;
  asm AND AL,3FH;
  asm MOV right,AL;
  asm IN AL,Port1C;
  asm MOV forward,AL;
  asm AND AL,3FH;
  asm MOV left,AL;
  asm MOV AL,forward;
  asm AND AL,0C0H;
  asm MOV forward,AL;
}              

void motors(void)
{
  asm MOV AL,03H;
  asm OUT Port1A,AL;
  for(l=0;l<200;l++);
  asm MOV AL,00;
  asm OUT Port1A,AL;
  
}

void motorL(void)
{
  asm MOV AL,02H;
  asm OUT Port1A,AL;
  for(l=0;l<200;l++);
  asm AND AL,00;
  asm OUT Port1A,AL;
  
}

void motorR(void)
{
  asm MOV AL,01H;
  asm OUT Port1A,AL;
  for(l=0;l<200;l++);
  asm MOV AL,00;
  asm OUT Port1A,AL;
  
}

void rightforward()
{
  asm AND AL,08H;
  asm MOV AH,14H;
  asm OR AL,AH;
  asm OUT Port1A,AL;
  asm MOV AH,0FH;
  asm AND AL,AH;
  asm OUT Port1A,Al;
}

void rightbackward()
{
  asm AND AL,08H;
  asm MOV AH,10H;
  asm OR AL,AH;
  asm OUT Port1A,AL;
  asm MOV AH,08H;
  asm AND AL,AH;
  asm OUT Port1A,AL;
}

void leftbackward()
{ 
  asm AND AL,04H;
  asm MOV AH,18H;
  asm OR AL,AH;
  asm OUT Port1A,AL;
  asm MOV AH,0CH;
  asm AND AL,AH;
  asm OUT Port1A,AL;
}

void leftforward()
{
  asm AND AL,0F7H;
  asm MOV AH,10H;
  asm OR AL,AH;
  asm OUT Port1A,AL;
  asm MOV AH,04H;
  asm AND AL,AH;
  asm OUT Port1A,AL;

}




void initialize2 (void)
{
}




/*** acceleration/deceleration routines ***/

void accelerate (void)
{       checked=0;
	step_inc=(start-finish)/pulses;
	delay=start;
	for (count=1; count<=pulses; count ++)
	{       motors();
		wait();
		delay=delay-step_inc;
		if (doing_now==going_forward) 
		{       if (checked < check_freq) checked++;
			else
			{       checked=0;
				check_centering();
				Pattern1=0;
				Show_Pattern();
			}
			if (forward_blocked() == true) count=pulses;
		}
	}
	delay=finish;
}

void decelerate (void)
{       checked=0;
	step_inc=(finish-start)/pulses;
	delay=start;
	for (count=1; count<=pulses; count ++)
	{       motors();
		wait();
		delay=delay+step_inc;
		if (doing_now==going_forward) 
		{       if (checked < check_freq) checked++;
			else
			{       checked=0;
				check_centering();
				Pattern1=0;
				Show_Pattern();
			}
			if (forward_blocked() == true) count=pulses;
		}
	}
	delay=finish;
}


/*** turn left/right/around, go forward routines ***/

void turn_left (void)
{      doing_now=turning_left;
	delay=turn_full;
	leftbackward();
	rightforward();
	start=stop;
	finish=turn_full;
	pulses=turn_pulses_a;
	accelerate();
	start=delay;
	finish=delay;
	pulses=turn_pulses_b;
	accelerate();
	start=delay;
	finish=stop;
	pulses=turn_pulses_c;
	decelerate();
	
     /*   for (i=0; i < pulses_t; i++)
	{  motors();
	   wait();
	} */
	delay=stop;
}

void turn_right (void)
{       doing_now=turning_right;
	leftforward();
	rightbackward();
	delay=turn_full;
	start=stop;
	finish=turn_full;
	pulses=turn_pulses_a;
	accelerate();
	start=delay;
	finish=delay;
	pulses=turn_pulses_b;
	accelerate();
	start=delay;
	finish=stop;
	pulses=turn_pulses_c;
	decelerate();
	
     /*   for (i=0; i < pulses_t; i++) 
	{       motors();
		wait();
	} */
	delay=stop;
}

void turn_around (void)
{       doing_now=turning_around;
	leftforward();
	rightbackward();
	delay=around_full;
	start=stop;
	finish=around_full;
	pulses=around_pulses_a;
	accelerate();
	start=delay;
	finish=delay;
	pulses=around_pulses_b;
	accelerate();
	start=delay;
	finish=stop;
	pulses=around_pulses_c;
	decelerate();
	
     /*   for (i=0; i < pulses_a; i++)
	{       motors();
		wait();
	} */
	delay=stop;
}

void go_forward (void)
{       doing_now=going_forward;
	leftforward();
	rightforward();
	delay=forward_full;
	start=stop;
	finish=forward_full;
	pulses=forward_pulses_a;
	accelerate();
	start=delay;
	finish=delay;
	pulses=forward_pulses_b;
	accelerate();
	start=delay;
	finish=stop;
	pulses=forward_pulses_c;
	decelerate();
	
      /*  for (i=0; i < (pulses_f*k); i++) 
	{       motors();
		wait();
	} */
	delay=stop;
}


/*** search junction routines ***/

void search_junction (void)
{       i=0;
	if (north_blocked()) ty=ty+8;
else    {       i=1;
		if (!north_blocked()) exi=exi+1;
	}
	i=0;
	if (south_blocked()) ty=ty+4;
else    {       i=1;
		if (!south_blocked()) exi=exi+1;
	}
	i=0;
	if (east_blocked()) ty=ty+2;
else    {       i=1;
		if (!east_blocked()) exi=exi+1;
	}
	i=0;
	if (west_blocked()) ty=ty+1;
else    {       i=1;
		if (!west_blocked()) exi=exi+1;
	}
	if (exi != 0) exi=exi-1;
	map[x][y].number=count;
	map[x][y].typ=ty;
	map[x][y].exits=exi;
	map[x][y].old=true;
}


/*** go north/south/east/west routines ***/

void go_north (void)
{       switch(facing)
	{       case south:     turn_around();
				break;
		case east:      turn_left();
				break;
		case west:      turn_right();
				break;
		default:        ;
	}
	facing=north;
	go_forward();
}

void go_south (void)
{       switch(facing)
	{       case north:     turn_around();
				break;
		case east:      turn_right();
				break;
		case west:      turn_left();
				break;
		default:        ;
	}
	facing=south;
	go_forward();
}

void go_east (void)
{       switch(facing)
	{       case north:     turn_right();
				break;
		case south:     turn_left();
				break;
		case west:      turn_around();
				break;
		default:        ;
	}
	facing=east;
	go_forward();
}

void go_west (void)
{       switch(facing)
	{       case north:     turn_left();
				break;
		case south:     turn_right();
				break;
		case east:      turn_around();
				break;
		default:        ;
	}
	facing=north;
	go_forward();
}


/*** map_blocked function ***/

short int map_blocked (void)
{       current=map[temp_x][temp_y].typ;
	switch (o)
	{       case north:     mask=8;
				break;
		case south:     mask=4;
				break;
		case east:      mask=2;
				break;
		case west:      mask=1;
				break;
		default:        ;
	}
	if ((current & mask)==0) return true;
	else return false;
}


/*** checking for the destination square ***/

void dest_found (void)
{       dest_x=x;
	dest_y=y;
}

void check_dest (void)
{       coord=map[x][y].number-3;

	if (map[x-1][y].number==coord) 
	{       i=0;
		if (west_blocked()==false) dest_found(); 
	}
else    if (map[x+1][y].number==coord)
	{       i=0;
		if (east_blocked()==false) dest_found(); 
	}
}


/*** moving and backtracking routines ***/

void move2 (void)
{       switch(item1)
	{       case north:     go_north();
				break;
		case south:     go_south();
				break;
		case east:      go_east();
				break;
		case west:      go_west();
				break;
		default:        ;
	}
}

void move (void)
{  if (done_flag==false) stack_num=1;
	else stack_num=0;
	stack_number=0;
	create();
	if (done_flag==false)
	{       stack_number=0;
		while (is_empty()==false)
		{  item1=stack_top();
			stack_number=1;
			push();
			stack_number=0;
			pop();
		}
	}
	stack_number=stack_num;
	while (is_empty()==false)
	{       k=1;
		item1=255;
		item2=255;
		t=true;
		stack_number=stack_num;
		item1=stack_top();
		pop();
		while ((t==true) & (is_empty()==false))
		{       item2=stack_top();
			if (item2==item1)
			{       k++;
				pop();
			}
			else t=false;
		}
		move2();
	}
	stack_number=0;
	create();
}

void backtrack (void)
{  stack_number=0;
	create();
	i=x;
	j=y;
	count=map[i][j].number;
	while (map[i][j].exits==0)
	{  through=0;
		o=north;
		temp_x=i;
		temp_y=j;
		if ((map[i][j+1].number==(count-1)) & (map_blocked()==false)) 
		{  stack_number=0;
			newitem=north;
			if (done_flag==false) push();
			else 
			{       newitem=south;
				push();
			}
			j++;
			through=1;
		}
		if (through==0)
		{  o=south;
			if ((map[i][j-1].number==(count-1)) & (map_blocked()==false)) 
			{  newitem=south;
				if (done_flag==false) push();
				else 
				{  newitem=north;
					push();
				}
				j--;
				through=1;
			}
		}  
		if (through==0)
		{  o=east;
			if ((map[i+1][j].number==(count-1)) & (map_blocked()==false)) 
			{  newitem=east;
				if (done_flag==false) push();
				else 
				{  newitem=west;
					push();
				}
				i++;
				through=1;
			}
		}
		if (through==0)
		{  o=west;
			if ((map[i-1][j].number==(count-1)) & (map_blocked()==false))
			{  newitem=west;
				if (done_flag==false) push();
				else 
				{  newitem=east;
					push();
				}
			i--;
			}
		}
		count--;
	}
	move();
	x=i;
	y=j;
	count--;
}

short int twotothe (void)
/* returns b=twotothe(a) */
{       b=1;
	while (a > 0)
	{       b=b*2;     
		a--;
	}
	return b;
}

short int sensormask (void)
/* returns c=2^(6-d) */
{       a=(6-d);
	return twotothe();
}

/*** main program ***/

void main (void)
{
	asm ORG 0000H;
  /* initialize stack segment and stack pointer */
  asm MOV AX,0000H;
  asm MOV SS,AX;
  asm MOV AX,7FFFH;
  asm MOV SP,AX;

  /*Initialize data segment */
  asm MOV AX,0100H;
  asm MOV DS,AX;

  Initialize();

/* Continue Main Here */



	x=1;
	y=1;
	dest_x=0;
	dest_y=0;
	done_flag=false;
	facing=north;
	count=1;
	initialize2();

	/** first run **/

	while (true)
	{       Pattern1=0;
		Show_Pattern();
		go_forward();
		makemasks();
		a=0;
		if (r6 == 0) a++;
		if (r5 == 0) a++; 
		if (r4 == 0) a++; 
		if (r3 == 0) a++; 
		if (r2 == 0) a++; 
		if (r1 == 0) a++; 
		if (a == 6) turn_right();
		else if (forward_blocked() == false) a=6;
		else 
		{       a=0;
			if (l6 == 0) a++;
			if (l5 == 0) a++;
			if (l4 == 0) a++;
			if (l3 == 0) a++;
			if (l2 == 0) a++; 
			if (l1 == 0) a++; 
			if (a == 6) turn_left();
		}
		if (a != 6) turn_around();
	}

}

void Start_Up_Jump(void)
{
  asm ORG 7FF0H;
  asm DB 0EAH;
  asm DB 00H;
  asm DB 00H;
  asm DB 00H;
  asm DB 0F8H;
}







