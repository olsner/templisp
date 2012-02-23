//#include <stdio.h>
int printf(const char *fmt, ...);
#include "templ_lists.h"

inline const char * bool2str(bool b) { return (b)?"true":"false"; }

int main()
{
	typedef icons<1, icons<3, icons<2, inil> > > mylist;
	typedef insert<insert<insert<inil, 1>, 2>, 3> mysortedlist;
	print<mylist> printOut;
	print<mysortedlist> sortedPrintOut;
	terminate<print_int<3> > three;
	printf("List: %s.\n", (char *)printOut);
	printf("Three: %s.\n", (char *)three);
	printf("Sorted: %s.\n", (char *)sortedPrintOut);
	printf("same_type: %s.\n", bool2str(same_type<mysortedlist::cdr::cdr, nil<int> >::value));
	printf("Member (3, 4): (%s, %s)\n",
		bool2str(member<mysortedlist, 3>::value),
		bool2str(member<mysortedlist, 4>::value));
	typedef remove<mysortedlist, 3>::type remove3;
	print <remove3> remove3print;
	printf("Remove 3: %s.\n", (char *)remove3print);
	printf("Length remove3: %d\n", length<remove3>::value);
	printf("Length mysortedlist: %d\n", length<mysortedlist>::value);	
	printf("Length mylist: %d\n", length<mylist>::value);
	
	typedef reverse<remove3>::type reverse_remove3;
	print <reverse_remove3> reverse_remove3print;
	printf("Reverse Remove 3: %s.\n", (char *)reverse_remove3print);
	
	typedef append<mylist, mysortedlist>::type appended;
	print <appended> appendedprint;
	printf("Append mylist++mysortedlist: %s.\n", (char *)appendedprint);
}
