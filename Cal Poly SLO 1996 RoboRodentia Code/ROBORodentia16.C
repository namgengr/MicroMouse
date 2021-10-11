/*************************************************************************/
/* includes */
/*************************************************************************/

#include <io51.h>

/*************************************************************************/
/* defines */
/*************************************************************************/

#define true    1
#define false   0
#define left    2
#define right   3
#define speed_f 30 /*35*/
#define speed_s 75 /*75*/

#define pulse   2
#define check_freq      10

#define one     1
#define two     2

#define both    22
#define none    0

#define off_either 1
#define off_both   2
#define off_right  3
#define off_left   4
#define on_both    5
#define on_right   6
#define on_left    7
#define on_either  8

/*************************************************************************/
/* prototypes */
/*************************************************************************/

void first_run(void);
void everything(void);

void fwd_p(void);
void left_p(void);
void rght_p(void);
void back_p(void);

void init();

void fl_fr(void);
void hl_hr(void);
void hl_fr(void);
void fl_hr(void);

int left_sensors(void);
int right_sensors(void);


void catch_on(void);
void catch_off(void);
void catch_dir_up(void);
void catch_dir_down(void);
void catch_pulse(int d);
void catch_up(void);
void catch_down(void);

int rear(void);
int front(void);
void wait(int d); 
int read_switch(void);

void turn_left(void);
void turn_right(void);

int drifting(void);
int l_drifting(void);
int r_drifting(void);

void back_left(int walls);
void back_right(int walls);
void end_backup(void);

void diagonal(int w);
int drifting_d(void);
void diag1(int w);
void diag2(void);
void diag3(void);
void diag4(void);
void diag5(void);
void diag6(void);

void diag4b(void);
void diag5b(void);
void diagonalb(int w);

void go_forward_steps(int count);
void r_go_forward_steps(int count);
void go_back_steps(int count);

/*************************************************************************/
/* variables */
/*************************************************************************/

int balls;

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

void back_p()
{       /* square wave generator w/50% duty cycle. The frequency 
	   of the pulse is controlled by the timers with a 30uS pulse width. 
	   The complete routine constitutes one square-wave cycle. */
	P3.0 = 0;
	P3.1 = 0;
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

/************************/
/* READ SWITCHED        */
/************************/

int read_switch()
{
	int swtch;

	swtch=(P2 & 0xF0)/16;
	swtch=0x0F-swtch;
	return(swtch);
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
/* wait (delay) routines and catcher stuff */
/*************************************************************************/

void wait(int d)
{       int i;
	for (i=0; i<d; i++);
} 


int rear()
{       int ret;
	if (P0.5 == 1) ret=true;
	else ret=false;
	return(ret);
}

int front()
{       int ret;       
	if (P0.6 == 1) ret=true;
	else ret=false;
	return(ret);
}

void catch_on()
{       P0.2=0;
}

void catch_off()
{       P0.2=1;
}

void catch_dir_up()
{       P1.6=1;
}

void catch_dir_down()
{       P1.6=0;
}

void catch_pulse(int d)
{       P2.0=1;
	wait(d);
	P2.0=0;
}

void catch_up()
{       int i;
	catch_on();
	catch_dir_up();
	for (i=0;i<400;i++) catch_pulse(60); 
	catch_off();
}

void catch_down()
{       int i,j;
	j=0;
	catch_on();
	catch_dir_down();
	for (i=0;i<80;i++) {fwd_p(); wait(85);} 
	for (i=0;i<400;i++) catch_pulse(40);
	catch_off();
}
 
void init()
{       P0.2=1;
	P2.2=1;
}

int right_sensors()
{       int ret;
	ret=(8*P0.0)+(4*P0.1)+(2*P3.2)+(1*P0.4);
	return(ret);
}

int left_sensors()
{       int ret;
	ret=(8*P0.7)+(4*P0.3)+(2*P2.3)+(1*P2.1);
	return(ret);
}

void release()
{       P2.2=0;
		wait(30000);
		P2.2=1;
}

/********************************/
/* DRIFTING ROUTINES            */
/********************************/

int drifting()
{       int ret;
	int l,r;

	ret=false;
	l=left_sensors();
	r=right_sensors();

	if      ((l==0x02) & (r==0x02)) ret=left;
	else if ((l==0x04) & (r==0x04)) ret=right;
	else if ((r==0x01)) ret=left;
	else if ((r==0x02)) ret=left;
	else if ((r==0x04)) ret=right;
	else if ((r==0x08)) ret=right;
	else if ((l==0x01)) ret=left;
	else if ((l==0x02)) ret=left;
	else if ((l==0x04)) ret=right;
	else if ((l==0x08)) ret=right;
	return(ret);
}

int r_drifting()
{       int ret;
	int l,r;

	ret=false;
	l=left_sensors();
	r=right_sensors();

	if ((r==0x01)) ret=left;
	else if ((r==0x02)) ret=left;
	else if ((r==0x03)) ret=left;
	else if ((r==0x04)) ret=right;
	else if ((r==0x08)) ret=right;
	else if ((r==12)) ret=right;
	return(ret);
}



int l_drifting()
{       int ret;
	int l,r;

	ret=false;
	l=left_sensors();
	r=right_sensors();

	if ((l==0x01)) ret=left;
	else if ((l==0x02)) ret=left;
	else if ((l==0x03)) ret=left;
	else if ((l==0x04)) ret=right;
	else if ((l==0x08)) ret=right;
	else if ((l==12)) ret=right;
	return(ret);
}



void end_backup()
{       int i;
	int steps;
	int lsteps, rsteps;
	int temp;
	steps=12;
	lsteps=0;
	rsteps=0;
	temp=0;
	while (rear() == false)
	{       if (steps==0)
		{       if (drifting() == left)
			{       fl_hr();
				for (i=0;i<2;i++) { back_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=2;
				rsteps=0;
			}
			else if (drifting() == right)
			{       hl_fr();
				for (i=0;i<2;i++) { back_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=2;
				lsteps=0;
			}
			else {  back_p(); 
				wait(speed_f);
				 }
			steps=12;
		}
		else {  steps--;
			back_p();
			wait(speed_f);
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				back_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				back_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}
}



/*      *       *       *       *       *       */
/* going forward routines                        */
/*      *       *       *       *       *       */

void go_forward_steps(int count)
{       int i;
	int c;
	int steps;
	int lsteps, rsteps;
	int temp;
	steps=check_freq;
	lsteps=0;
	rsteps=0;
	temp=0;
	for (c=0;c<count;c++)
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
		       
		}
		
		if (steps==0)
		{       if (drifting() == left)
			{       fl_hr();
				for (i=0;i<pulse;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=pulse;
				rsteps=0;
			}
			else if (drifting() == right)
			{       hl_fr();
				for (i=0;i<pulse;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=pulse;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait(speed_f);
				 }
			steps=10;
		}
		else {  steps--;
			fwd_p();
			wait(speed_f);
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}

}

void r_go_forward_steps(int count)
{       int i;
	int c;
	int steps;
	int lsteps, rsteps;
	int temp;
	steps=check_freq;
	lsteps=0;
	rsteps=0;
	temp=0;
	for (c=0;c<count;c++)
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
		       
		}
		
		if (steps==0)
		{       if (r_drifting() == left)
			{       fl_hr();
				for (i=0;i<pulse;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=pulse;
				rsteps=0;
			}
			else if (r_drifting() == right)
			{       hl_fr();
				for (i=0;i<pulse;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=pulse;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait(speed_f);
				 }
			steps=10;
		}
		else {  steps--;
			fwd_p();
			wait(speed_f);
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}

}



void go_back_steps(int count)
{       int i;
	int c;
	int steps;
	int lsteps, rsteps;
	int temp;
	steps=check_freq;
	lsteps=0;
	rsteps=0;
	temp=0;
	for (c=0;c<count;c++)
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
		       
		}
		
		if (steps==0)
		{       if (drifting() == left)
			{       fl_hr();
				for (i=0;i<pulse;i++) { back_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=pulse;
				rsteps=0;
			}
			else if (drifting() == right)
			{       hl_fr();
				for (i=0;i<pulse;i++) { back_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=pulse;
				lsteps=0;
			}
			else {  back_p(); 
				wait(speed_f);
				 }
			steps=10;
		}
		else {  steps--;
			back_p();
			wait(speed_f);
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				back_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				back_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}

}



void go(int out_cond, int check, int w, int pulses)
{       int i;
	int lsteps, rsteps;
	int temp;
	int steps;
	int out_check;        
	int check_cond;

	steps=check_freq;
	lsteps=0;
	rsteps=0;
	temp=0;

   if ((pulses != 0) | (out_cond == 0))
   {    
	for (i=0;i<pulses;i++)
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
			w=100;
		}
		
		switch(check)
		{       case left:      check_cond=l_drifting();
					break;
			case right:     check_cond=r_drifting();
					break;
			case both:      check_cond=drifting();
					break;
			case none:      check_cond=false;
					break;
		}

		if (steps==0)
		{       if (check_cond == left)
			{       fl_hr();
				for (i=0;i<pulse;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=pulse;
				rsteps=0;
			}
			else if (check_cond == right)
			{       hl_fr();
				for (i=0;i<pulse;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=pulse;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait((speed_f+w));
				if (w!=0) w--;
				 }
			steps=check_freq;
		}
		else {  steps--;
			fwd_p();
			wait((speed_f+w));
			if (w!=0) w--;
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}
   }
   else if ((out_cond !=0) & (pulses == 0))
   {    switch(out_cond)
	{       case off_either:  out_check=(left_sensors() !=0) & (right_sensors() != 0);
				break;
		case off_both:  out_check=(left_sensors() !=0) | (right_sensors() != 0);
				break;
		case off_right: out_check=(right_sensors() != 0);
				break;
		case off_left:  out_check=(left_sensors() != 0);
				break;
		case on_right:  out_check=(right_sensors() == 0);
				break;
		case on_left:   out_check=(left_sensors() == 0);
				break;
		case on_both:   out_check=(left_sensors() == 0) | (right_sensors() == 0);
				break;
		case on_either: out_check=(left_sensors() == 0) & (right_sensors() == 0);
				break;
	}             
   
	while (out_check)
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
			w=100;
		}
		
		switch(check)
		{       case left:      check_cond=l_drifting();
					break;
			case right:     check_cond=r_drifting();
					break;
			case both:      check_cond=drifting();
					break;
			case none:      check_cond=false;
					break;
		}

		if (steps==0)
		{       if (check_cond == left)
			{       fl_hr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=2;
				rsteps=0;
			}
			else if (check_cond == right)
			{       hl_fr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=2;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait((speed_f+w));
				if (w!=0) w--;
				 }
			steps=10;
		}
		else {  steps--;
			fwd_p();
			wait((speed_f+w));
			if (w!=0) w--;
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
		switch(out_cond)
		{       case off_either:  out_check=(left_sensors() !=0) & (right_sensors() != 0);
					break;
			case off_both:  out_check=(left_sensors() !=0) | (right_sensors() != 0);
				       break;
			case off_right: out_check=(right_sensors() != 0);
					 break;
			case off_left:  out_check=(left_sensors() != 0);
					break;
			case on_right:  out_check=(right_sensors() == 0);
					break;
			case on_left:   out_check=(left_sensors() == 0);
					break;
			case on_both:   out_check=(left_sensors() == 0) | (right_sensors() == 0);
					break;
			case on_either: out_check=(left_sensors() == 0) & (right_sensors() == 0);
					break;
		}             
	}

	     if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 

	
   }
}

void back(int out_cond, int check, int w, int pulses)
{       int i;
	int lsteps, rsteps;
	int temp;
	int steps;
	int out_check;        
	int check_cond;

	steps=check_freq;
	lsteps=0;
	rsteps=0;
	temp=0;

   if (pulses != 0)
   {    
	for (i=0;i<pulses;i++)
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
			w=100;
		}
		
		switch(check)
		{       case left:      check_cond=l_drifting();
					break;
			case right:     check_cond=r_drifting();
					break;
			case both:      check_cond=drifting();
					break;
			case none:      check_cond=false;
					break;
		}

		if (steps==0)
		{       if (check_cond == left)
			{       fl_hr();
				for (i=0;i<pulse;i++) { back_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=pulse;
				rsteps=0;
			}
			else if (check_cond == right)
			{       hl_fr();
				for (i=0;i<pulse;i++) { back_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=pulse;
				lsteps=0;
			}
			else {  back_p(); 
				wait((speed_f+w));
				if (w!=0) w--;
				 }
			steps=check_freq;
		}
		else {  steps--;
			back_p();
			wait((speed_f+w));
			if (w!=0) w--;
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				back_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				back_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}
   }
   else 
   {    switch(out_cond)
	{       case off_either:  out_check=(left_sensors() !=0) & (right_sensors() != 0);
				break;
		case off_both:  out_check=(left_sensors() !=0) | (right_sensors() != 0);
				break;
		case off_right: out_check=(right_sensors() != 0);
				break;
		case off_left:  out_check=(left_sensors() != 0);
				break;
		case on_right:  out_check=(right_sensors() == 0);
				break;
		case on_left:   out_check=(left_sensors() == 0);
				break;
		case on_both:   out_check=(left_sensors() == 0) | (right_sensors() == 0);
				break;
		case on_either: out_check=(left_sensors() == 0) & (right_sensors() == 0);
				break;
	}             
   
	while (out_check)
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
			w=100;
		}
		
		switch(check)
		{       case left:      check_cond=l_drifting();
					break;
			case right:     check_cond=r_drifting();
					break;
			case both:      check_cond=drifting();
					break;
			case none:      check_cond=false;
					break;
		}

		if (steps==0)
		{       if (check_cond == left)
			{       fl_hr();
				for (i=0;i<2;i++) { back_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=2;
				rsteps=0;
			}
			else if (check_cond == right)
			{       hl_fr();
				for (i=0;i<2;i++) { back_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=2;
				lsteps=0;
			}
			else {  back_p(); 
				wait((speed_f+w));
				if (w!=0) w--;
				 }
			steps=10;
		}
		else {  steps--;
			back_p();
			wait((speed_f+w));
			if (w!=0) w--;
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				back_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				back_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
		switch(out_cond)
		{       case off_either:  out_check=(left_sensors() !=0) & (right_sensors() != 0);
					break;
			case off_both:  out_check=(left_sensors() !=0) | (right_sensors() != 0);
				       break;
			case off_right: out_check=(right_sensors() != 0);
					 break;
			case off_left:  out_check=(left_sensors() != 0);
					break;
			case on_right:  out_check=(right_sensors() == 0);
					break;
			case on_left:   out_check=(left_sensors() == 0);
					break;
			case on_both:   out_check=(left_sensors() == 0) | (right_sensors() == 0);
					break;
			case on_either: out_check=(left_sensors() == 0) & (right_sensors() == 0);
					break;
		}             
	}

	    if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
	
	
   }
}



/*************************************************************************/
/* backwards turns */
/*************************************************************************/

void back_right(int walls)
{       int i;
	int j;
	int k;
	int far,near,steps;
	far=350; steps=350; near=170; 
	i=((far/near)-1);
	k=2;
	j=k;
	for (i=0;i<1;i++) {back_p(); wait(125);}  
	for (i=0;i<265;i++)
	{       hl_fr();
		back_p();
		wait(80);
		if (j==0)
		{       hl_hr();
			back_p();
			wait(80);
			j=k;
		}
		else j--;
	}
	hl_hr();
	if (walls==two) back(on_both,none,0,0);
	else back(on_either,none,0,0);
}

void back_left(int walls)
{       int i;       
	int something;

	something=120;
	for (i=0;i<something;i++)
	{       hl_hr();
		back_p();
		wait((speed_f+20));
	}

	wait(2000);
	
	for (i=1; i<88; i++)
	{       left_p();
		wait(165);
	}
	
	if (walls==two) back(on_both,none,100,0);
	else back(on_either,none,100,0);
}

/*************************************************************************/
/* forward turns */
/*************************************************************************/

void turn_right()
{       int i;       
	int something;

	something=114; /*118*/
	for (i=0;i<something;i++)
	{       hl_hr();
		fwd_p();
		wait((speed_f+20));
	}

	wait(1000);
	for (i=1; i<91; i++) /*90*/
	{       rght_p();
		wait(165);
	}
	wait(1000);
	go(on_both,none,100,0);
}

void turn_left()
{       int i;
	int j;
	int k;
	int far,near,steps;
	far=350; steps=350; near=170; 
	i=((far/near)-1);

	k=2;
	j=k;
	for (i=0;i<1;i++) {fwd_p(); wait(125);}  

	for (i=0;i<265;i++)
	{       hl_fr();
		fwd_p();
		wait(80);
		if (j==0)
		{       hl_hr();
			fwd_p();
			wait(80);
			j=k;
		}
		else j--;
	}
	hl_hr();
	go(on_both,left,100,0);
}


/***************************************************************************/
/* Diagonal routines                                                       */
/***************************************************************************/

/* right_turn();
cross_street();
diag_1();                initial forward + little bit
diag_2();                angle turn 
diag_3();                forward until walls 
diag_4();                through the diagonal + little bit 
diag_5();                turn 
diag_6();                forward to intersection 

drifting_d();

*/


int drifting_d()
{       int ret;
	int l,r;

	ret=false;
	l=left_sensors();
	r=right_sensors();

	if      ((l==0x01) & (r==0x01)) ret=left;
	else if ((l==0x02) & (r==0x02)) ret=left;
	else if ((l==0x04) & (r==0x04)) ret=right;
	else if ((l==0x08) & (r==0x08)) ret=right;
	 /* else if ((l==0x02)) ret=left;
	else if ((r==0x02)) ret=left;
	else if ((l==0x04)) ret=right;
	else if ((r==0x04)) ret=right; 
	 */
	return(ret);
}


void diag1(int w)    /* like go_forward with drifting_d() in place of drifting() */
{       int i;
	int steps;
	int lsteps, rsteps;
	int temp;
	steps=10;
	lsteps=0;
	rsteps=0;
	temp=0;
	while ((left_sensors() !=0) & (right_sensors() != 0))
	{       if (steps==0)
		{       if (drifting_d() == left)
			{       fl_hr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=2;
				rsteps=0;
			}
			else if (drifting_d() == right)
			{       hl_fr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=2;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait(speed_f+w);
				if (w!=0) w--;
				 }
			steps=10;
		}
		else {  steps--;
			fwd_p();
			wait(speed_f+w);
			if (w!=0) w--;
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}
	for (i=0;i<10;i++) 
	{       fwd_p();
		wait((speed_f));
	}

	wait(1000);
}

void diag2()    /* little right turn */
{       int i;
	for (i=0;i<54;i++)
	{       rght_p();
		wait(165);
	}
	wait(1000);
	for (i=0;i<20;i++)
	{       fwd_p();
		wait(speed_f);
	}

}
	 
void diag3()    /* cross_street */
{       int i;
	int w;
	int steps, lsteps, rsteps, temp;
	
	steps=10; lsteps=0; rsteps=0;
	w=100;
	while ((left_sensors() == 0) | (right_sensors() == 0))
	{       if (steps==0)
		{       if (drifting() == left)
			{       fl_hr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=2;
				rsteps=0;
			}
			else if (drifting() == right)
			{       hl_fr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=2;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait(speed_f+w);
				if (w!=0) w--;
				 }
			steps=10;
		}
		else {  steps--;
			fwd_p();
			wait(speed_f+w);
			if (w!=0) w--;
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
	}       
}


void diag4()    /* like go_forward with drifting_d() in place of drifting()*/
{       int i;
	int steps;
	int lsteps, rsteps;
	int temp;
	int w;
	steps=10;
	lsteps=0;
	rsteps=0;
	temp=0;
	while ((left_sensors() !=0) & (right_sensors() != 0))
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
			w=100;
		}
	
		if (steps==0)
		{       if (drifting_d() == left)
			{       fl_hr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=2;
				rsteps=0;
			}
			else if (drifting_d() == right)
			{       hl_fr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=2;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait(speed_f);
				 }
			steps=10;
		}
		else {  steps--;
			fwd_p();
			wait(speed_f);
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}

	for (i=0;i<214;i++) /* 304 */
	{       fwd_p();
		wait((speed_f+20));
	}

}

void diag4b()    /* like go_forward with drifting_d() in place of drifting()*/
{       int i;
	int steps;
	int lsteps, rsteps;
	int temp;
	int w;
	w=0;

	steps=10;
	lsteps=0;
	rsteps=0;
	temp=0;
	while ((left_sensors() !=0) & (right_sensors() != 0))
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
			w=100;
		}
	
		if (steps==0)
		{       if (drifting_d() == left)
			{       fl_hr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=2;
				rsteps=0;
			}
			else if (drifting_d() == right)
			{       hl_fr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=2;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait(speed_f+w);
				if (w!=0) w--; 
				 }
			steps=10;
		}
		else {  steps--;
			fwd_p();
			wait(speed_f+w);
			if (w!=0) w--;
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}

	for (i=0;i<99;i++)   /* 102 */
	{       fwd_p();
		wait((speed_f+20));
	}

	

}



void diag5()    /* big right turn */
{       int i;
	for (i=0;i<125;i++)  /*127*/
	{       rght_p();
		wait(165);
	}
}

void diag5b()    /* little right turn */
{       int i;
	for (i=0;i<34;i++) /* 16 */
	{       rght_p();
		wait(165);
	}

	wait(1000);

	/* alley_one(); */

	/* for (i=0;i<5;i++)   87 
	{       back_p();
		wait(speed_f);
	} */

	wait(1000);


}




void diagonal(int w)
{       diag1(w);
	diag2();
	diag3();
	diag4();
	diag5();
}

void diagonalb(int w)
{       diag1(w);
	diag2();
	diag3();
	diag4b();
	diag5b();
}

void l_forward(int w)
{       int i;
	int steps;
	int lsteps, rsteps;
	int temp;
	steps=10;
	lsteps=0;
	rsteps=0;
	temp=0;
	while ((left_sensors() !=0) & (right_sensors() != 0))
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
			w=100;
		}
		
		if (steps==0)
		{       if (l_drifting() == left)
			{       fl_hr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=2;
				rsteps=0;
			}
			else if (l_drifting() == right)
			{       hl_fr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=2;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait((speed_f+w));
				if (w!=0) w--;
				 }
			steps=10;
		}
		else {  steps--;
			fwd_p();
			wait((speed_f+w));
			if (w!=0) w--;
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}
}

void r_forward(int w)
{       int i;
	int steps;
	int lsteps, rsteps;
	int temp;
	steps=10;
	lsteps=0;
	rsteps=0;
	temp=0;
	while ((right_sensors() != 0))
	{       if (front() == true)
		{       catch_up();
			balls++;
			if (balls != 2) catch_down();
			w=100;
		}
		
		if (steps==0)
		{       if (r_drifting() == left)
			{       fl_hr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				lsteps+=2;
				rsteps=0;
			}
			else if (r_drifting() == right)
			{       hl_fr();
				for (i=0;i<2;i++) { fwd_p(); wait(speed_s);} 
				hl_hr();
				rsteps+=2;
				lsteps=0;
			}
			else {  fwd_p(); 
				wait((speed_f+w));
				if (w!=0) w--;
				 }
			steps=10;
		}
		else {  steps--;
			fwd_p();
			wait((speed_f+w));
			if (w!=0) w--;
			 }
		
		if ((lsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=lsteps/2;
			while (temp > 0)
			{       hl_fr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		}
		if ((rsteps!=0) & ((left_sensors()==6) | (right_sensors()==6)))
		{       temp=rsteps/2;
			while (temp > 0)
			{       fl_hr();
				fwd_p();
				wait(speed_s);
				temp--;
			}
				hl_hr();
				lsteps=0;
				rsteps=0;
		} 
			   
	
	}
}



/****************************************************************/
/*                  Main                                        */
/****************************************************************/

void everything() 
{       int i;

	go(off_right,both,100,0);
	turn_right();
	
	go(on_both,both,100,0);
	go(off_right,both,0,0);
	go(on_both,left,0,0);
	go(off_right,both,0,0);
	
	wait(500);

	back(on_both,left,100,0);
	back(off_either,both,100,0);
	back(on_both,left,0,0);
	
	wait(500);

	go(off_either,both,100,0);
	turn_right();

	go(on_both,both,100,0);
	go(off_either,both,0,0);
	
	go(off_left,both,0,0);
	go(on_right,right,0,0);
	go(off_both,both,0,0);
	turn_right();

	go(on_right,right,100,0);
	go(off_right,right,0,0);
	turn_right();

	go(on_right,both,100,0);
	go(off_right,both,0,0);
	turn_right();

	go(on_right,both,100,0);
	go(off_both,both,0,0);
	turn_left();

	go(on_both,both,0,0);
	go(off_both,both,0,0);

	wait(500);

	back(on_both,both,100,0);
	back(off_left,both,100,0);
	back(on_left,right,0,0);
	back(off_left,both,0,0);

	wait(500);

	go(on_both,both,100,0);
	go(off_right,both,0,0);
	turn_right();

	go(on_both,both,100,0);
	go(off_both,both,0,0);
	turn_right();

	go(on_right,right,100,0);
	go(off_right,right,0,0);
	go(on_right,none,0,0);
	go(off_both,right,0,0);
	turn_right();

	go(on_both,both,100,0);
	go(off_right,both,0,0);
	go(on_both,left,0,0);
	
	go_forward_steps(300);
	
	wait(500);

	back(off_either,left,100,0);
	P1=4;
	back(on_both,left,0,0);
	
	wait(500);

	go(off_right,left,100,0);
	turn_right();
	
	go(on_both,both,100,0);
	go(off_both,both,0,0);
	turn_right();

	go(on_right,both,100,0);
	go(off_right,both,0,0);
	turn_left();
	
	go(on_left,left,0,0);
	go(off_left,left,0,0);
	go(on_left,none,0,0);
	go(off_both,left,0,0);
	turn_right();

	go(on_both,both,100,0);
	go(off_right,both,0,0);
	turn_right();
	
	go(on_both,both,100,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_either,both,0,0);

	wait(500);

	back(on_both,both,100,0);
	back(off_either,both,100,0);
	back(on_both,left,0,0);
	back(off_right,both,0,0);

	wait(500);
	
	go(on_both,both,100,0);
	diagonal(100);
	go(on_right,none,100,0);
	
	r_go_forward_steps(200);
			     
	wait(500);

	back(off_right,right,100,0);
	back(on_right,none,0,0);
	back(off_both,right,0,0);
	back_right(2);

	back(on_both,both,100,0);
	
	go_back_steps(200);
	wait(500);
	
	go(off_left,both,100,0);
	go(on_both,both,0,0);
	go(off_left,both,0,0);

	wait(500);

	back(on_both,both,100,0);
	back(off_left,both,0,0);
	back(on_both,both,0,0);
	back(off_left,both,0,0);
	back(on_both,both,0,0);
	end_backup();
	for (i=0;i<25;i++)
	{       back_p();
		wait(speed_f);
	}
	release();       
}

void second_run()
{       int i;

	go(off_either,both,100,0);
	turn_right();
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_right,both,0,0);
	turn_right();
	go(on_both,both,0,0);
	go(off_right,right,0,0);
	go(on_right,none,0,0);
	go(off_right,right,0,0);
	turn_right();
	go(on_both,both,0,0);
	go(off_right,both,0,0);
	turn_right();
	go(on_both,both,0,0);
	go(off_right,both,0,0);
	turn_right();
	go_forward_steps(350);
	wait(500);
	back(off_both,both,100,0);
	back_right(1);
	back(on_both,left,0,0);
	go_back_steps(50);
	wait(500);
	go(off_right,both,0,0);
	go(on_right,left,0,0);
	go(off_right,both,0,0);
	turn_right();
	go(on_right,right,0,0);
	go(off_right,right,0,0);
	turn_right();
	go(on_both,both,0,0);
	go(off_right,both,0,0);
	turn_right();
	go(on_both,both,0,0);
	go(off_both,both,0,0);
	turn_left();
	go(on_both,both,0,0);
	go(off_both,both,0,0);
	turn_right();
	go(on_both,both,0,0);
	go(off_right,both,0,0);
	go(on_both,left,0,0);
	go(off_right,both,0,0);
	turn_right();
	diagonalb(0);
	turn_left();
	go(on_both,left,0,0);
	go(off_both,left,0,0);
	turn_right();
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	
	go_forward_steps(300);
	
	wait(500);

	back(off_either,both,150,0);
	back_left(2);
	back(off_both,both,0,0);
	go(on_both,none,100,0);
	go(off_both,both,100,0);
	turn_left();
	
	go_forward_steps(200);
	
	wait(500);

	back(off_either,both,150,0);
	back(on_both,right,0,0);
	
	end_backup();
	for (i=0;i<25;i++)
	{       back_p();
		wait(speed_f);
	}
	release();       
}

void third_run()
{       int i;

	go(off_either,both,100,0);
	turn_right();
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	turn_right();

	go(on_both,both,100,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_either,both,0,0);

	wait(500);

	back(on_both,both,100,0);
	back(off_either,both,100,0);
	back(on_both,left,0,0);
	back(off_right,both,0,0);

	wait(500);
	
	go(on_both,both,100,0);

	diagonal(100);
	go(on_right,right,100,0);
	go(off_both,right,100,0);
	turn_right();
	go(on_both,both,0,0);
	wait(500);
	back(off_right,both,100,0);
	back(on_right,both,100,0);
	back(off_right,both,0,0);
	back(on_both,both,0,0);
	wait(500);
	go(off_right,both,100,0);
	turn_right();
	go(on_both,both,0,0);
	go(off_right,both,0,0);
	turn_right();
	go(on_both,both,0,0);
	go(off_left,both,0,0);
	turn_left();
	go(on_both,both,0,0);
	go(off_left,both,0,0);
	turn_left();
	go(on_left,both,0,0);
	go(off_left,both,0,0);
	go(on_right,both,0,0);
	go(off_right,both,0,0);
	turn_right();
	go(on_right,right,0,0);
	go(off_right,right,0,0);
	turn_right();
	go(on_right,both,0,0);
	go(off_right,both,0,0);
	go(on_right,both,0,0);
	go_forward_steps(300);
	wait(500);
	back(off_right,both,100,0);
	back(on_right,both,0,0);
	go_back_steps(50);
	wait(500);
	go(off_right,both,100,0);
	turn_right();
	go(on_both,both,0,0);
	go(off_both,both,0,0);
	wait(500);
	back(on_both,both,100,0);
	back(off_both,both,100,0);
	back_right(2);
	end_backup();
	for (i=0;i<25;i++)
	{       back_p();
		wait(speed_f);
	}
	release();       
}

void first_run()
{       int i;

	go(off_either,both,100,0);
	turn_right();
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	turn_right();
	diagonalb(0);
	turn_left();
	go(on_both,left,0,0);
	go(off_both,left,0,0);
	turn_right();
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	
	go_forward_steps(300);
	
	wait(500);

	back(off_either,both,150,0);
	back_left(2);
	back(off_both,both,0,0);
	go(on_both,none,100,0);
	go(off_both,both,100,0);
	turn_left();
	
	go_forward_steps(200);
	
	wait(500);

	back(off_either,both,150,0);
	back(on_both,right,0,0);
	
	end_backup();
	for (i=0;i<25;i++)
	{       back_p();
		wait(speed_f);
	}
	release();       
}

void fourth_run()
{       int i;

	go(off_either,both,100,0);
	turn_right();
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	turn_right();
	
	go(on_both,both,100,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	go(off_either,both,0,0);

	wait(500);

	back(on_both,both,100,0);
	back(off_either,both,100,0);
	back(on_both,left,0,0);
	back(off_right,both,0,0);

	wait(500);
	
	go(on_both,both,100,0);
	
	diagonalb(0);
	turn_left();
	go(on_both,left,0,0);
	go(off_both,left,0,0);
	turn_right();
	go(on_both,left,0,0);
	go(off_either,both,0,0);
	go(on_both,left,0,0);
	
	go_forward_steps(300);
	
	wait(500);

	back(off_either,both,150,0);
	back_left(2);
	back(off_both,both,0,0);
	go(on_both,none,100,0);
	go(off_both,both,100,0);
	turn_left();
	
	go(on_both,both,0,0);
	go(off_left,both,0,0);
	go(on_both,both,0,0);
	go(off_left,both,0,0);
	wait(500);
	back(on_both,both,100,0);
	back(off_left,both,100,0);
	back(on_both,both,0,0);
	back(off_left,both,0,0);
	back(on_both,both,0,0);

	
	end_backup();
	for (i=0;i<25;i++)
	{       back_p();
		wait(speed_f);
	}
	release();       
}


main()
{                          /********************/
	int i;             /*    MAIN          */
	init();            /********************/
	hl_hr();
	balls=0;        
	P1=0;

	i=read_switch();
	switch(i)
	{       case 1: first_run();
			break;
		case 2: everything();
			break;
		case 3: second_run();
			break;
		case 4: third_run();
			break;
		case 5: fourth_run();
			break;
	}
	
}

	


