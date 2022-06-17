import java.util.Scanner;

class LCD_HUD
{
	public static void main(String[] args)
	{
		int mode = 0;
		int green_timeDelay = 5;
		int yellow_timeDelay = 2;
		boolean TOD_enable = false;
		int TOD_m1;
		int TOD_m2;
		int TOD_time;
		
		int cursor = 0;
		boolean enter = false;
		
		//Stage 1
		String s1[] = {"MODE", "TIME Delay", "TOD mode switch"};
		String s_MODE[] = {"Urban", "Rural", "Y_EW/R_NS", "Y_NS/R_EW", "RED"};
		
		System.out.println("Current MODE: " + mode_toString(mode));
		
		//print array
		while(enter == false)
		{
			for(int i=0;i<s1.length;i++)
			{
				System.out.println(s1[i]);
			}
			//update cursor
			cursor = cursorSet(s1, cursor);
			//update array
			s1 = updateArray(s1, cursor);
		}
		
		
		switch(cursor)
		{
			case 0: 				//MODE selected
					
					break;
			case 1: 				//Time delay selected
			
					break;
			case 2: 				//TOD mode switch selected
			
					break;
		}
	}

	
	
	public String mode_toString(int M)
	{
		switch(M)
		{
			case 0: return ("Urban"); break;
			case 1: return ("Rural"); break;
			case 2: return ("Y_EW/R_NS"); break;
			case 3: return ("Y_NS/R_EW"); break;
			case 4: return ("RED"); break;
		}
	}
	
	public String[] updateArray(String[] s, int c)
	{
		for(int i=0; i<s.length;i++)
		{
			if(c == i)
			{
				s[i] = ("->" + s[i]);
			}
			else
			{
				s[i] = s[i].replace("->", "");
			}
		}
	}
	
	public int cursorSet(String[] s, int c)
	{
		int max =  s.length - 1;
		
		//get input 
		Scanner k = new Scanner(System.in);
		System.out.println("Enter cursor input: ");
		String input = k.next();
		
		if(input == "up" && c != 0)
		{
			c--;
		}else if(input == "up" && c == 0)
		{
			c = max;
		}else if(input == "down" && c != max)
		{
			c++;
		}else if(input == "down" && c == max)
		{
			c = 0;
		}
	}
}