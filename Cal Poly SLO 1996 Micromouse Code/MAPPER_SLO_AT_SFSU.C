/*************************************************************************/
/*                                                                       */
/* Cal Poly, San Luis Obispo MICROMOUSE PROJECT code for 1996            */
/*                                                                       */
/*************************************************************************/

/*     Direction preference settings:   */
/*                                      */
/*      0       NS                      */
/*      1       NE                      */
/*      2       NW                      */
/*                                      */
/*      3       SN                      */
/*      4       SE                      */
/*      5       SW                      */
/*                                      */
/*      6       EN                      */
/*      7       ES                      */
/*      8       EW                      */
/*                                      */
/*      9       WN                      */
/*      10      WS                      */
/*      11      WE                      */




/*************************************************************************/
/* includes */
/*************************************************************************/

#include <io51.h>

/*************************************************************************/
/* defines */
/*************************************************************************/

#define maxstack 257

#define true 1
#define false 0

#define north 0
#define south 1
#define east 2
#define west 3

#define on 2
#define off 3

#define accel 1
#define decel 2
#define coast 3
#define scan_begin 130 

/*************************************************************************/
/* prototypes */
/*************************************************************************/

void fwd_p(void);
void left_p(void);
void rght_p(void);

int go_fwd(void);
void turn_rght(void);
void turn_left(void);
void turn_around(void);

int left_sensors(void);
int rght_sensors(void);
int read_switch(void);
int read_forward(void);
int drifting(void); 

void save_maze_info(void);
void reload_maze_info(void);

void wait(int d); 
int correct(void);

void fl_fr(void);
void hl_hr(void);
void hl_fr(void);
void fl_hr(void);

void create (int stack_number);
int is_empty (int stack_number);
void push (int stack_number, int newitem);
void pop (int stack_number);
int stack_top (int stack_number);

int forward_blocked(void);
int back_blocked(void);
int left_blocked(void);
int right_blocked(void);

int north_blocked(int i);
int east_blocked(int i);
int west_blocked(int i);
int south_blocked(int i);

void search_junction(void);

void go_north(void);
void go_south(void);
void go_east(void);
void go_west(void);

int map_blocked (int o, int temp_x, int temp_y);

void dest_found(void);
int check_dest(void);

void move2(int item1);
void move(void);
void backtrack(void);

void second_run(void);
void third_run(void);
void first_run(void);

void go(void);

int turn_coming(int left, int rght);

void check_switches (int param);

void go_dir1(void);
void go_dir2(void);
void go_dir3(void);
void go_dir4(void);

int dir1_blocked(int i);
int dir2_blocked(int i);
int dir3_blocked(int i);
int dir4_blocked(int i);

void wait_on_forward(void);


/*************************************************************************/
/* variables */
/*************************************************************************/

int check_freq,checked;                         /* error correction     */
int x,y,facing,count,square,dest_x,dest_y;      /* mapping              */
int done_flag,forward;                          /* backtracking         */
int straight_count,falling_l,falling_r;         /* fall-off-the-wall    */ 
int var_delay,mode;                             /* accel/decel          */
int max_delay=75; /*50*/     
int min_delay=30; /*30*/
int dir1, dir2, dir3, dir4;
int delay_change;
int mapping;
int restart;
short int old_exits[18][18];
int new_map_max_delay;
int new_map_min_delay;
int destination;

/*************************************************************************/
/* structs */
/*************************************************************************/

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

struct maptype map[18][18];
struct stack_type backstack,newstack;

/*************************************************************************/
/* stack procedures */
/*************************************************************************/

void create (int stack_number)
{       if (stack_number == 0) backstack.top=0;
	else if (stack_number == 1) newstack.top=0;
}

int is_empty (int stack_number)
{       int i;

	i=false;
	switch(stack_number)
	{       case 0:
			if (backstack.top == 0) i=true; 
			else i=false;
			break;
		case 1:
			if (newstack.top == 0) i=true; 
			else i=false;
			break;
		default: ;
	}
	return i;
}

void push (int stack_number, int newitem)
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

void pop (int stack_number)
{       switch(stack_number)
	{
		case 0:
			if (is_empty(0) == true) ;
			else backstack.top--;
			break;
		case 1:
			if (is_empty(1) == true) ;
			else newstack.top--;
			break;
		default: ;
	}
}

int stack_top (int stack_number)
{       int i;        
		switch(stack_number)
		{       
			case 0:
				if (is_empty(0) == true) ;
				else  i=backstack.items[backstack.top];
				break;
			case 1:
				if (is_empty(1) == true) ;
				else  i=newstack.items[newstack.top];
			default: ;
		}
		return i;
}

/*************************************************************************/
/* forward/back/left/right blocked routines */
/*************************************************************************/

int forward_blocked()
{       int ret;
	if (forward == 2) ret=true;
	else ret=false;
	return ret;
}

int back_blocked()
{       if ((x==1) & (y==1) & (facing==north)) return true;
	else return false;
}

int left_blocked()
{       int ret;
	int l=left_sensors();
	if (l == 0) ret=false;
	else ret=true;
	return ret;
}

int right_blocked()
{       int ret;
	int r=rght_sensors();
	if (r == 0) ret=false;
	else ret=true;
	return ret;
}

/*************************/
/* dirX_blocked(i) routines */
/*************************/

int dir1_blocked(int i)
{       int ret;
	switch(dir1)
	{       case north: ret=north_blocked(i); break; 
		case south: ret=south_blocked(i); break;
		case east:  ret=east_blocked(i);  break;
		case west:  ret=west_blocked(i);  break;
	}
	return(ret);
}


int dir2_blocked(int i)
{       int ret;
	switch(dir2)
	{       case north: ret=north_blocked(i); break; 
		case south: ret=south_blocked(i); break;
		case east:  ret=east_blocked(i);  break;
		case west:  ret=west_blocked(i);  break;
	}
	return(ret);
}


int dir3_blocked(int i)
{       int ret;
	switch(dir3)
	{       case north: ret=north_blocked(i); break; 
		case south: ret=south_blocked(i); break;
		case east:  ret=east_blocked(i);  break;
		case west:  ret=west_blocked(i);  break;
	}
	return(ret);
}


int dir4_blocked(int i)
{       int ret;
	switch(dir4)
	{       case north: ret=north_blocked(i); break; 
		case south: ret=south_blocked(i); break;
		case east:  ret=east_blocked(i);  break;
		case west:  ret=west_blocked(i);  break;
	}
	return(ret);
}


/*************************************************************************/
/* north/south/east/west blocked routines */
/*************************************************************************/




int north_blocked (int i)
{       int k;
	
	k=false;
	switch(i)
	{       case 0:
			switch(facing)
			   {       case north: k=forward_blocked();
					       break;
				   case south: k=back_blocked();
					       break;
				   case east:  k=left_blocked();
					       break;
				   case west:  k=right_blocked();
					       break;
				   default:    ;
			   }
			   break;
		case 1:
			if (map[x][y+1].old == true) k=true;
			else 
			   {    switch(facing)
				{       case north: k=forward_blocked();
						    break;
					case south: k=back_blocked();
						    break;
					case east:  k=left_blocked();
						    break;
					case west:  k=right_blocked();
						    break;
					default:    ;
				}
			   }
			break;
		case 2:
			if (map[x][y+1].old == true) k=true;
			else
			   {    k=false;
				if ((map[x][y].typ & 8) == 8) k=true;  
			   }
			break;
	}
	return k;
}

int south_blocked (int i)
{       int k;
	
	k=false;
	switch(i)
	{       case 0:
			switch(facing)
			   {       case north: k=back_blocked();
					       break;
				   case south: k=forward_blocked();
					       break;
				   case east:  k=right_blocked();
					       break;
				   case west:  k=left_blocked();
					       break;
				   default:    ;
			   }
			   break;
		case 1:
			if (map[x][y-1].old == true) k=true;
			else 
			   {    switch(facing)
				{       case north: k=back_blocked();
						    break;
					case south: k=forward_blocked();
						    break;
					case east:  k=right_blocked();
						    break;
					case west:  k=left_blocked();
						    break;
					default:    ;
				}
			   }
			break;
		case 2:
			if (map[x][y-1].old == true) k=true;
			else
			   {    k=false;
				if ((map[x][y].typ & 4) == 4) k=true;  
			   }
			break;
	}
	return k;
}


int east_blocked (int i)
{       int k;
	
	k=false;
	switch(i)
	{       case 0:
			switch(facing)
			   {       case north: k=right_blocked();
					       break;
				   case south: k=left_blocked();
					       break;
				   case east:  k=forward_blocked();
					       break;
				   case west:  k=back_blocked();
					       break;
				   default:    ;
			   }
			   break;
		case 1:
			if (map[x+1][y].old == true) k=true;
			else 
			   {    switch(facing)
				{       case north: k=right_blocked();
						    break;
					case south: k=left_blocked();
						    break;
					case east:  k=forward_blocked();
						    break;
					case west:  k=back_blocked();
						    break;
					default:    ;
				}
			   }
			break;
		case 2:
			if (map[x+1][y].old == true) k=true;
			else
			   {    k=false;
				if ((map[x][y].typ & 2) == 2) k=true;  
			   }
			break;
	}
	return k;
}

int west_blocked (int i)
{       int k;
	
	k=false;
	switch(i)
	{       case 0:
			switch(facing)
			   {       case north: k=left_blocked();
					       break;
				   case south: k=right_blocked();
					       break;
				   case east:  k=back_blocked();
					       break;
				   case west:  k=forward_blocked();
					       break;
				   default:    ;
			   }
			   break;
		case 1:
			if (map[x-1][y].old == true) k=true;
			else 
			   {    switch(facing)
				{       case north: k=left_blocked();
						    break;
					case south: k=right_blocked();
						    break;
					case east:  k=back_blocked();
						    break;
					case west:  k=forward_blocked();
						    break;
					default:    ;
				}
			   }
			break;
		case 2:
			if (map[x-1][y].old == true) k=true;
			else
			   {    k=false;
				if ((map[x][y].typ & 1) == 1) k=true;  
			   }
			break;
	}

	return k;
}

/*************************************************************************/
/* search juntion routine */
/*************************************************************************/

void search_junction (void)
{       int ty,exi;
	ty=0;
	exi=0;
	
	if (north_blocked(0) == true) ty=ty+8;
else    {       if (north_blocked(1) == false) exi=exi+1;
	}
		
	if (south_blocked(0) == true) ty=ty+4;
else    {      if (south_blocked(1) == false) exi=exi+1;
	}
		
	if (east_blocked(0) == true) ty=ty+2;
else    {      if (east_blocked(1) == false) exi=exi+1;
	}
		
	if (west_blocked(0) == true) ty=ty+1;
else    {      if (west_blocked(1) == false) exi=exi+1;
	}
	
	if (exi != 0) exi=exi-1;
	map[x][y].number=count;
	map[x][y].typ=ty;
	map[x][y].exits=exi;
	map[x][y].old=true;
}

/*********************/
/* go_dirX routines  */
/*********************/

void go_dir1(void)
{       switch(dir1)
	{       case north: y++; go_north(); break;
		case south: y--; go_south(); break;
		case east:  x++; go_east(); break;
		case west:  x--; go_west(); break;
	}
}


void go_dir2(void)
{       switch(dir2)
	{       case north: y++; go_north(); break;
		case south: y--; go_south(); break;
		case east:  x++; go_east(); break;
		case west:  x--; go_west(); break;
	}
}


void go_dir3(void)
{       switch(dir3)
	{       case north: y++; go_north(); break;
		case south: y--; go_south(); break;
		case east:  x++; go_east(); break;
		case west:  x--; go_west(); break;
	}
}


void go_dir4(void)
{       switch(dir4)
	{       case north: y++; go_north(); break;
		case south: y--; go_south(); break;
		case east:  x++; go_east(); break;
		case west:  x--; go_west(); break;
	}
}


/*************************************************************************/
/* go north/south/east/west routines */
/*************************************************************************/

void go_north (void)
{       int i;
	switch(facing)
	{       case south:     turn_around();
				break;
		case east:      turn_left();
				break;
		case west:      turn_rght();
				break;
		default:        ;
	}
	facing=north;
	i=go_fwd();
}

void go_south (void)
{       int i;
	switch(facing)
	{       case north:     turn_around();
				break;
		case east:      turn_rght();
				break;
		case west:      turn_left();
				break;
		default:        ;
	}
	facing=south;
	i=go_fwd();
}

void go_east (void)
{       int i;
	switch(facing)
	{       case north:     turn_rght();
				break;
		case south:     turn_left();
				break;
		case west:      turn_around();
				break;
		default:        ;
	}
	facing=east;
	i=go_fwd();
}

void go_west (void)
{       int i;
	switch(facing)
	{       case north:     turn_left();
				break;
		case south:     turn_rght();
				break;
		case east:      turn_around();
				break;
		default:        ;
	}
	facing=west;
	i=go_fwd();
}

/*************************************************************************/
/* map blocked routine */
/*************************************************************************/

int map_blocked (int o, int temp_x, int temp_y)
{       
	int current, mask;
	mask=0;

	current=map[temp_x][temp_y].typ;
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
	if ((current & mask) != 0) return true;
	else return false;
}

/*************************************************************************/
/* check destination square routines */
/*************************************************************************/

void dest_found (void)
{       dest_x=x;
	dest_y=y;
	x=1;y=1;facing=south;
	destination=true;
}

int check_dest (void)
{       int coord;
	int ret;

	ret=false;
	
	coord=map[x][y].number-3;

	if (destination==true) ;

else    if (map[x-1][y].number == coord) 
	{       if (map_blocked(west,x,y) == false) 
		{       dest_found(); 
			ret=true;
		}
	}
else    if (map[x+1][y].number == coord)
	{       if (map_blocked(east,x,y) == false) 
		{       dest_found(); 
			ret=true;
		}
	}
else    if (map[x][y+1].number == coord)
	{       if (map_blocked(north,x,y) == false) 
		{       dest_found(); 
			ret=true;
		}
	}
else    if (map[x][y-1].number == coord)
	{       if (map_blocked(south,x,y) == false) 
		{       dest_found(); 
			ret=true;
		}
	}

	return(ret);
}

/*************************************************************************/
/* moving and backtracking routines */
/*************************************************************************/

void move2 (int item1)
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
{       int stack_num,stack_number;  
	int newitem;
	int k,t;
	int item1,item2;

	if (done_flag == false) stack_num=1;
	else stack_num=0;
	newstack.top=0;
	if (done_flag==false)
	{       stack_number=0;
		while (is_empty(stack_number) == false)
		{  item1=stack_top(stack_number);
		   newitem=item1;
			stack_number=1;
			push(stack_number,newitem);
			stack_number=0;
			pop(stack_number);
		}
	}
	stack_number=stack_num;
	while (is_empty(stack_number) == false)
	{       k=1;
		item1=255;
		item2=255;
		t=true;
		stack_number=stack_num;
		item1=stack_top(stack_number);
		pop(stack_number);
		while ((t == true) & (is_empty(stack_number) == false))
		{       item2=stack_top(stack_number);
			if (item2 == item1)
			{       k++;
				pop(stack_number);
			}
			else t=false;
		}
/*!*/           square=k;
/*!*/           move2(item1);
		if ((left_sensors() == 0x28) & (rght_sensors() == 0x05)) goto finish2; 
	}
	stack_number=0;
	backstack.top=0;

finish2: ;
}

void backtrack (void)
{       int stack_number,ii,jj,temp_x,temp_y,o,through,newitem;
	
	mapping=false;
	
	stack_number=0;
	backstack.top=0;
	ii=x;
	jj=y;
	count=map[ii][jj].number;
	while (map[ii][jj].exits == 0)
	{       through=0;
		o=north;
		temp_x=ii;
		temp_y=jj;
		if ((map[ii][jj+1].number == (count-1)) & (map_blocked(o,temp_x,temp_y) == false)) 
		     {  stack_number=0;
			newitem=north;
			if (done_flag == false) push(stack_number,newitem);
			else 
			{       newitem=south;
				push(stack_number,newitem);
			}
			jj++;
			through=1;
		}
		if (through == 0)
		{  o=south;
		   if ((map[ii][jj-1].number == (count-1)) & (map_blocked(o,temp_x,temp_y) == false)) 
			{  newitem=south;
				if (done_flag == false) push(stack_number,newitem);
				else 
				{  newitem=north;
					push(stack_number,newitem);
				}
				jj--;
				through=1;
			}
		}  
		if (through == 0)
		{  o=east;
			if ((map[ii+1][jj].number == (count-1)) & (map_blocked(o,temp_x,temp_y) == false))
			{  newitem=east;
				if (done_flag == false) push(stack_number,newitem);
				else 
				{  newitem=west;
					push(stack_number,newitem);
				}
				ii++;
				through=1;
			}
		}
		if (through == 0)
		{  o=west;
		   if ((map[ii-1][jj].number == (count-1)) & (map_blocked(o,temp_x,temp_y) == false)) 
			{  newitem=west;
				if (done_flag == false) push(stack_number,newitem);
				else 
				{  newitem=east;
					push(stack_number,newitem);
				}
			ii--;
			}
		}
		count--;
	}
	move();
	x=ii;
	y=jj;
	/* count--; */ 
	count=map[x][y].number; 
	if (map[x][y].exits != 0) map[x][y].exits--;
}

/*************************************************************************/
/* second run routine */
/*************************************************************************/

void second_run (void)
{
	int changed,a,b,c,d,e;
	changed=true;
	x=1;
	y=1;
	save_maze_info();
	while (changed == true)
	{       changed=false;
		for (x=1; x < 17; x++)
			for (y=1; y < 17; y++)
			{       map[x][y].exits=0;    /* new */
				a=map[x][y+1].number;
				b=map[x][y-1].number;
				c=map[x+1][y].number;
				d=map[x-1][y].number;
				e=map[x][y].typ;
				count=map[x][y].number;
				if ((a > (count+1)) & ((e & 8) == 0)) 
				{       if (map[x][y+1].number != 255) 
					{       map[x][y+1].number=count+1;
						changed=true;
					}
				}
				if ((b > (count+1)) & ((e & 4) == 0))
				{       if (map[x][y-1].number != 255) 
					{       map[x][y-1].number=count+1;
						changed=true;
					}
				}
				if ((c > (count+1)) & ((e & 2) == 0))
				{       if (map[x+1][y].number != 255) 
					{       map[x+1][y].number=count+1;
						changed=true;
					}
				}
				if ((d > (count+1)) & ((e & 1) == 0))
				{       if(map[x-1][y].number != 255)
					{       map[x-1][y].number=count+1;
						changed=true;
					}
				}

			}
	}
	/* wait(5000); */
}

/*************************************************************************/
/* third run routine */
/*************************************************************************/

void third_run (void)
{
	/*** third run ***/

	restart=false;
	wait_on_forward();
	check_switches(3);
	wait_on_forward();
	check_switches(4);
	wait_on_forward();
	check_switches(5);

	while (true)
	{       mode=accel;
		delay_change=9;
		facing=north;
		done_flag=true;
		x=dest_x;
		y=dest_y;
		map[1][1].exits=1;
		backtrack();
		
		if (restart==true) break; /* out of while loop and routine */
		else 
		{       wait_on_forward();
			check_switches(3);
			wait_on_forward();
			check_switches(4);
			wait_on_forward();
			check_switches(5);
			wait(5000);
		}
	}
}


void wait_on_forward(void)
{       int fs;
	fs=read_forward();
	while (fs != 1) fs=read_forward();
	while (fs != 0) fs=read_forward();
}

void check_switches (int param)
{       int sw;
	sw=read_switch();
	switch(param)
	{      case 5:
			switch(sw)                     
			{       case 0: new_map_max_delay=75; /*40*/
					new_map_min_delay=30;
					break;
				case 1: 
					new_map_max_delay=75; /*35*/
					new_map_min_delay=25;
					break;
				case 2: new_map_max_delay=75;
					new_map_min_delay=20;
					break;
				case 3: new_map_max_delay=75;
					new_map_min_delay=15;
					break;
				case 4: new_map_max_delay=75; /*30*/
					new_map_min_delay=10;
					break;
				case 5: new_map_max_delay=75;
					new_map_min_delay=5;
					break;
				case 6: new_map_max_delay=75;
					new_map_min_delay=3;
					break;
				case 7: new_map_max_delay=75;
					new_map_min_delay=1;
					break;
				case 8: new_map_max_delay=75;
					new_map_min_delay=0;
					break;
				default: new_map_max_delay=75; /*40*/
					 new_map_min_delay=30;
					break;
			}
			break;
		case 4:
			switch(sw)
			{       case 0: restart=false;
					break;
				case 1: restart=true;
					break;
			}
			break;
		case 3:
			switch(sw)                     
			{       case 0: max_delay=75; /*40*/
					min_delay=30;
					break;
				case 1: 
					max_delay=75; /*35*/
					min_delay=25;
					break;
				case 2: max_delay=75;
					min_delay=20;
					break;
				case 3: max_delay=75;
					min_delay=15;
					break;
				case 4: max_delay=75; /*30*/
					min_delay=10;
					break;
				case 5: max_delay=75;
					min_delay=5;
					break;
				case 6: max_delay=75;
					min_delay=3;
					break;
				case 7: max_delay=75;
					min_delay=1;
					break;
				case 8: max_delay=75;
					min_delay=0;
					break;
				default: max_delay=75; /*40*/
					 min_delay=30;
					break;
			}
			break;

		case 1:
			switch(sw)
			{       case 0:
				case 1:
				case 2:         dir1=north;
						break;
				case 3:
				case 4:
				case 5:         dir1=south;
						break;    
				case 6:
				case 7:
				case 8:         dir1=east;
						break;
				case 9:
				case 10:
				case 11:        dir1=west;
						break;
			}
			switch(sw)
			{       case 3:
				case 6:
				case 9:         dir2=north;
						break;
				case 0:
				case 7:
				case 10:        dir2=south;
						break;
				case 1:
				case 4:
				case 11:        dir2=east;
						break;
				case 2:
				case 5:
				case 8:         dir2=west;
						break;
			}
			break;

		case 2:
			switch(sw)
			{       case 0:
				case 1:
				case 2:         dir3=north;
						break;
				case 3:
				case 4:
				case 5:         dir3=south;
						break;    
				case 6:
				case 7:
				case 8:         dir3=east;
						break;
				case 9:
				case 10:
				case 11:        dir3=west;
						break;
			}
			switch(sw)
			{       case 3:
				case 6:
				case 9:         dir4=north;
						break;
				case 0:
				case 7:
				case 10:        dir4=south;
						break;
				case 1:
				case 4:
				case 11:        dir4=east;
						break;
				case 2:
				case 5:
				case 8:         dir4=west;
						break;
			}
			break;


	}  /* end of switch(param) */
}



/*************************************************************************/
/* first run routine */
/*************************************************************************/

void first_run (void)
{
	int i;
	int out;

	out=false;

	if (restart==false) 
	{       wait_on_forward();      /* change the speed */
		check_switches(3);
	
		wait_on_forward();      /* change direction preference (1st half) */
		check_switches(1);
	
		wait_on_forward();      /* change direction preference (2nd half) */
		check_switches(2);
	
		search_junction();
	}
	while ((x != 1) | (y != 1) | (facing != south))
	{       mapping=true;
		map[1][1].exits=1;
		i=2;
		square=1;
		if (dir1_blocked(i) == true) 
			if (dir2_blocked(i) == true)
				if (dir3_blocked(i) == true)
					if (dir4_blocked(i) == true)
					{       out=check_dest();
						if (out == false) backtrack();
					}        
					else 
					{       count++;
						go_dir4();
					}
				else
				{       count++;
					go_dir3();
				}
			else 
			{       count++;
				go_dir2();
			}
		else
		{       count++;
			go_dir1();
		}
	straight_count++;
	}
}


/*************************************************************************/
/* map setup */
/*************************************************************************/

void go(void)
{       int i,j;       

	P1=255;
	x=1;
	y=1;
	dest_x=0;
	dest_y=0;
	done_flag=false;
	facing=north;
	square=1;
	count=1;

	for (i=0; i < 18; i++)
		for (j=0; j < 18; j++)
		{       map[i][j].number=255;
			map[i][j].old=false;
			map[i][j].exits=0;
			map[i][j].typ=15;
		}

	create(0);
	create(1);

	var_delay=max_delay;
	
}

/*************************************************************************/
/* fwd_p, etc. routine */
/*************************************************************************/

void fwd_p()
{       /* square wave generator w/50% duty cycle. The frequency 
	   of the pulse is controlled by the timers with a 30uS pulse width. 
	   The complete routine constitutes one square-wave cycle. */
	P3.0 = 1;
	P3.1 = 1;
	P3 |= 0x30;
	wait(1);
	P3 &= 0xCF;
	wait(1);
}

void left_p()
{       /* square wave generator w/50% duty cycle. The frequency 
	   of the pulse is controlled by the timers with a 30uS pulse width. 
	   The complete routine constitutes one square-wave cycle. */
	P3.0 = 0;
	P3.1 = 1;
	P3 |= 0x30;
	wait(5);
	P3 &= 0xCF;
	wait(5);
}

void rght_p()
{       /* square wave generator w/50% duty cycle. The frequency 
	   of the pulse is controlled by the timers with a 30uS pulse width. 
	   The complete routine constitutes one square-wave cycle. */
	P3.0 = 1;
	P3.1 = 0;
	P3 |= 0x30;
	wait(5);
	P3 &= 0xCF;
	wait(5);
}

/*************************************************************************/
/* half-step / full-step routines */
/*************************************************************************/

void fl_fr()
{                      /* function sets motors to full step mode */ 
	P3.6 = 0;
	P3.7 = 0;
}

void hl_hr()
{                       /* function sets motors to half step mode */ 
	P3.6 = 1;
	P3.7 = 1;
}

void hl_fr()
{                       /* function sets motors to half step mode */ 
	P3.6 = 1;
	P3.7 = 0;
}

void fl_hr()
{                       /* function sets motors to half step mode */ 
	P3.6 = 0;
	P3.7 = 1;
}

/*************************************************************************/
/* wait (delay) routine */
/*************************************************************************/

void wait(int d)
{       int i;
	for (i=0; i<d; i++);
} 

/*************************************************************************/
/* drifting error check routine */
/*************************************************************************/

int drifting()
{       
	int Al, Ar, Aret;
	
/* 1= ok
   2= 2 walls, drifting left
   3= 2 walls, drifting right 
   4= 1 wall, drifting left
   5= 1 wall, drifting right */

	Aret=1;
	Al=left_sensors();
	Ar=rght_sensors();
	     if ((Al==0x04) & (Ar==0x04)) Aret=4;
	else if ((Al==0x01) & (Ar==0x01)) Aret=4;
	else if ((Al==0x06) & (Ar==0x06)) Aret=4;
	else if ((Al==0x02) & (Ar==0x02)) Aret=4;
	else if ((Al==0x08) & (Ar==0x08)) Aret=5; 
	else if ((Al==0x18) & (Ar==0x18)) Aret=5;   
	else if ((Al==0x10) & (Ar==0x10)) Aret=5;
	else if ((Al==0x20) & (Ar==0x20)) Aret=5;
	else {  /* if ((Al==0x04) | (Ar==0x04)) Aret=4;
		else */ if ((Al==0x06) | (Ar==0x06)) Aret=4;
		else if ((Al==0x02) | (Ar==0x02)) Aret=4;
		/* else if ((Al==0x08) | (Ar==0x08)) Aret=5; */
		else if ((Al==0x18) | (Ar==0x18)) Aret=5;   
		else if ((Al==0x10) | (Ar==0x10)) Aret=5;
		else if ((Al==0x20) | (Ar==0x20)) Aret=5;
		else if ((Al==0x01) | (Ar==0x01)) Aret=4;
	     }
	return(Aret);
}  

/*************************************************************************/
/* correct error checking routine */
/*************************************************************************/

int correct()
{       int d2, i, ret;
	checked=0;
	d2=drifting();
		if (d2 == 4)
		{       for (i=0; i<3; i++)
			{       fl_hr();
				fwd_p();
				wait(var_delay+35); /* 40 */
			}
			hl_hr();
		}
		else if (d2 == 5)
		{       for (i=0; i<3; i++)
			{       hl_fr();
				fwd_p();
				wait(var_delay+35); /* 40 */
			}
			hl_hr();
		}
		
		if ((d2 == 4) | (d2 == 5)) ret=1;
		else ret=0;
		return(ret);
}

/*************************************************************************/
/* go forward */
/*************************************************************************/

int go_fwd()
{       int i, fwd_max,f,rs,ls,temp;

	int walls_checked;
	int left=false;
	int rght=false;
	int accel_count=0;
	int decel_count=0;      
	int searched;
	int fall_checked;
	
	fwd_max=242;
	searched=false;
	
	fall_checked=false;
	straight_count=1;

   while (square != 0)
   {    square--;     
   
	walls_checked=false;
	i=fwd_max;
	forward=0;
	temp=0;
	f=0;
	
	while (i > 0) 
	{       fwd_p();
		wait(var_delay);

		if ((mapping==true) & (i<69) & (searched==false)) 
		{       search_junction(); 
			searched=true;
		}

		/* falling off the wall setup */
		if ( (i< 69) & (fall_checked==false) )
		{       if (left_blocked()==true) falling_l=off; 
			else falling_l=false;
			if (right_blocked()==true) falling_r=off;
			else falling_r=false;
			fall_checked=true;
		}

		/* accel/decel code */
		if ((i == 242) & (var_delay == max_delay)) 
		{       mode=accel;
			delay_change=9;
		}

		switch(mode)
		{       case accel:     if (var_delay > min_delay) 
					{       accel_count++;
						if (accel_count == 7) 
						{       var_delay=var_delay-delay_change;
							if (var_delay>((max_delay-min_delay)/2)) delay_change=9;
							else delay_change=3;
							accel_count=0;
						}
					}
					else    
					{       var_delay=min_delay;
						mode=coast;
					}
					break;
			case decel:     if (var_delay < max_delay) 
					{       decel_count++;
						if (decel_count == 7) /* 4 */
						{       var_delay=var_delay+delay_change;
							if (var_delay>((max_delay-min_delay)/2)) delay_change=9;
							else delay_change=3;
							decel_count=0;
						}
					}
					else    
					{       var_delay=max_delay;
						mode=coast;
					}
					break;
			case coast:     ;
		}
				

		if (walls_checked == false)                                             
			if (i < scan_begin)                                             
			{       walls_checked=true;                                     
			
				if (left_sensors() != 0) left=true;                     
				else left=false;
			
				if (rght_sensors() != 0) rght=true;                     
				else rght=false;
			
				if (turn_coming(left,rght)==true) 
				{       mode=decel;                                 
					delay_change=3;
				}
			}

		
			if (forward == 0) forward=read_forward(); 
			else if (forward == 1)
			{       forward=2;
				i=85;
				if (mapping==true) search_junction();
				mode=decel;
				delay_change=3;
			}
		
			if (checked < check_freq) 
			{       checked++;
			}       
			else 
			{       f=correct(); 
				if (f == 1) 
				{       if (i > 3) i=i-3;
					f=0;
				}
			}
		
			/*falling off (or on) the wall routine*/
			if (straight_count >= 1)
			{       if (falling_r==off)
				{       rs=rght_sensors();
					if (rs == 0) 
					{       i=190; /*190*/
						straight_count=0;
					}
				}
				if (falling_l==off)
				{       ls=left_sensors();                        
					if (ls == 0)
					{       i=190; /*190*/
						straight_count=0;
					}
				}
			}

			if ((left_sensors() == 0x28) & (rght_sensors() == 0x05)) goto finish; 
		
			i--;   
		}

		/* more accel/decel code */
		if (mode == accel) 
		{       var_delay=min_delay;
			mode=coast;
		}
	
		if (mode == decel)
		{       var_delay=max_delay;
			mode=coast;
		}
   }
   temp=forward;
   return(temp);

finish: ;
}


/*************************************************************************/
/* turn left/right/around routines */
/*************************************************************************/

void turn_rght()
{       int i; 
	int rght_change;
	int rght_delay;

	/* wait(1000); */
	rght_delay=180;
	
	for (i=1; i<88; i++)    /*88*/
	{       if (i<30) rght_change=-4;
	   else if (i<40) rght_change=-2;
	   else if (i<50) rght_change=0;
	   else if (i<60) rght_change=2;
	   else rght_change=4;
		
		rght_delay=rght_delay+rght_change;

		rght_p();
		wait(rght_delay);
	}
	straight_count=0;
	var_delay=max_delay;
	/* wait(1000); */

	falling_l=false;
	falling_r=false;

}

void turn_left()
{       int i;
	int left_change;
	int left_delay;

	/* wait(1000); */
	left_delay=180;

	for (i=1; i<88; i++)    /*88*/
	{       if (i<30) left_change=-4;
	   else if (i<40) left_change=-2;
	   else if (i<50) left_change=0;
	   else if (i<60) left_change=2;
	   else left_change=4;
	
		left_delay=left_delay+left_change;

		left_p();
		wait(left_delay);
	}
	straight_count=0;
	var_delay=max_delay;
	/* wait(1000); */

	falling_l=false;
	falling_r=false;
}

void turn_around()
{       int i;
	int around_change;
	int around_delay;

	/* wait(1000); */
	around_delay=180;

	for (i=1; i<176; i++)   /*176*/
	{       if (i<30) around_change=-4;
	   else if (i<45) around_change=-2;
	   else if (i<125) around_change=0;
	   else if (i<140) around_change=2;
	   else around_change=4;

		around_delay=around_delay+around_change;

		left_p();
		wait(around_delay);
	}
	straight_count=0;
	var_delay=max_delay;
	/* wait(1000); */

	falling_l=false;
	falling_r=false;

}

/*************************************************************************/
/* sensor reading routines */
/*************************************************************************/

int left_sensors()      
{
	int l1, l2, l3, l4, l5, l6, left;

	l1=(P0 & 0x01)*32;
	l2=(P0 & 0x02)*8;
	l3=(P0 & 0x04)*2;
	l4=(P0 & 0x08)/2;
	l5=(P0 & 0x10)/8;
	l6=(P0 & 0x20)/32;
	left=l1 | l2 | l3 | l4 | l5 | l6;
	/* left=0x3F-left; */
	return(left);   
}

int rght_sensors()
{
	int r5,r6,right;
	
	r5=(P0 & 0x80)/8;
	r6=(P0 & 0x40)/2;
	right=P2; 
	right=right & 0x0F;
	right=right | r5 | r6;
	/* right=0x3F-right; */
	return(right);
}
	
int read_forward()
{       int temp2;
	temp2=P3;
	temp2=(temp2 & 0x04)/4;
	temp2=1-temp2;
	if (temp2==0) temp2=1;
	else temp2=0;
	return(temp2);


}

int read_switch()
{
	int swtch;

	swtch=(P2 & 0xF0)/16;
	swtch=0x0F-swtch;
	return(swtch);
}

/*************************************************************************/
/* turn coming routine */
/*************************************************************************/

int turn_coming (int left, int rght)
{       int ret,d1,d2,d3,d4;
	if (square != 0) ret=false;
	else
	{       d1=dir1_blocked(1);
		d2=dir2_blocked(1);
		d3=dir3_blocked(1);
		d4=dir4_blocked(1);
		if (facing==dir1) {     if (d1==false) ret=false;
					else ret=true;
				  }
	   else if (facing==dir2) {     if ((d1==true) & (d2==false)) ret=false;
					else ret=true;
				  }
	   else if (facing==dir3) {     if ((d1==true) & (d2==true) & (d3==false)) ret=false;
					else ret=true;
				  }
	   else if (facing==dir4) {     if ((d1==true) & (d2==true) & (d3==true) & (d4==false)) ret=false;
					else ret=true;
				  }
	
	if (mapping==false) ret=true;
	
	}
      
	return(ret);
}

void save_maze_info(void)
{       int i;       
	int j;

	for (i=0; i < 18; i++)
		for (j=0; j < 18; j++)
			old_exits[i][j]=map[i][j].exits;
}

void reload_maze_info(void) 
{       int i;
	int j;

	for (i=0; i < 18; i++)
		for (j=0; j < 18; j++)
			map[i][j].exits=old_exits[i][j];
}





/*************************************************************************/
/* MAIN PROGRAM */
/*************************************************************************/

main()
{
	int l,r,f;  
	destination=false;
	restart=false;
	checked=0;
	delay_change=9;
	straight_count=0;
	check_freq=0;
	falling_l=false;
	falling_r=false;
	/* f=go_fwd();   
	l=left_sensors();
	r=rght_sensors(); */
	go();        
	while (true)
	{
		first_run();
		second_run();
		third_run(); 
		reload_maze_info();
		done_flag=false;
		x=dest_x;
		y=dest_y;
		count=map[x][y].number;
		max_delay=new_map_max_delay;
		min_delay=new_map_min_delay;
	}

}

