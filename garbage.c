/*
 *Autor: Filip Gulan xgulan00@stud.fit.vutbr.cz
 */

#include "garbage.h"

trash TrashX;

void trashInit()
{
	
 	TrashX.Act = NULL;
 	TrashX.First = NULL;
 	TrashX.Last = NULL;
}

void addToBin(void *adresa)
{
 	trash_element pomocny = malloc(sizeof(struct elementSTrash));
	if(pomocny == NULL) //chyba alokacie
	{
		//
	}
	else //alokacia prebehla spravne
	{
		pomocny->adresa = adresa; //nahrame data
		pomocny->lptr = NULL; //prvok nalavo od pomocneho bude ukazovat na null
		pomocny->rptr = TrashX.First; //prvok napravo od pomocneho bude ukazovat na prvy prvok
		if(TrashX.Last == NULL) //ak to je prve vlozenie
		{
			TrashX.Last = pomocny; //posledny je nas vlozeny prvok
		}
		else //nieje prve vlozenie
		{
			TrashX.First->lptr = pomocny; //prvok nalavo prveho bude ukazovat na pomocny
		}
		TrashX.First = pomocny; //prvy je nas vlozeny prvok
	}
}

void myFree(void *adresa)
{
	TrashX.Act = TrashX.First;
	while(TrashX.Act != NULL)
	{
		if(TrashX.Act->adresa == adresa)
		{
			if(TrashX.Act == TrashX.First) //zoznam je aktivny a aktivny neni posledny
			{
				trash_element pomocny = TrashX.First; //dame si prvy do pomocnej nech ho mozme uvolnit neskor
				if(TrashX.First == TrashX.Last) //ak bol prvy zaroven posledny tak nastavime aktivitu a posledny tiez na nul
				{
					TrashX.First = NULL;
					TrashX.Last = NULL;
					TrashX.Act = NULL;
				}
				else
				{
					if(TrashX.First == TrashX.Act) //ak bol prvy aktivny, zrusime aktivitu tiez
					{
						TrashX.Act = NULL;
					}
					TrashX.First = pomocny->rptr; //prvym sa stava prvok napravo od prveho
					TrashX.First->lptr = NULL; //novy prvy prvok bude ukzovat nalavo na null
				}
				free(pomocny->adresa);
				free(pomocny);
				return;
			}
			if(TrashX.Act == TrashX.Last) //zoznam je aktivny a aktivny neni posledny
			{
				trash_element pomocny = TrashX.Last;
				if(TrashX.First == TrashX.Last) //ak bol prvy zaroven posledny tak nastavime aktivitu a posledny tiez na nul
				{
					TrashX.First = NULL;
					TrashX.Last = NULL;
					TrashX.Act = NULL;
				}
				else
				{
					if(TrashX.Last == TrashX.Act) //ak bol posledny aktivny, zrusime aktivitu tiez
					{
						TrashX.Act = NULL;
					}
					TrashX.Last = pomocny->lptr; //posledny sa stava prvok nalavo od posleneho 
					TrashX.Last->rptr = NULL; //novy posledny bude napravo ukazovat na null
				}
				free(pomocny->adresa);
				free(pomocny);
				return;
			}
			else
			{
				trash_element pomocny = TrashX.Act;
				TrashX.Act->lptr->rptr = TrashX.Act->rptr;
				TrashX.Act->rptr->lptr = TrashX.Act->lptr;
				free(pomocny->adresa);
				free(pomocny);
				return;
			}
		}
		TrashX.Act = TrashX.Act->rptr;
	}
}

void trashDestroy()
{
 	while(TrashX.First != NULL) //prejde vsetky prvky zoznamu
	{
		if(TrashX.First == TrashX.Act) //ak je ruseny prvok aktivny, tak zrusi aktivitu 
		{
			TrashX.Act = NULL;
		}
		if(TrashX.First == TrashX.Last) //ak je prvy zaroven poslednym tak zrusi ukazovanie na posledny
		{
			TrashX.Last = NULL;
		}
		trash_element pomocny = TrashX.First; //dame prvy do pomocnej aby sme ho mohli uvolnit
		TrashX.First = TrashX.First->rptr; //prvok napravo od prveho sa stava prvym
		free(pomocny->adresa);
		free(pomocny);
	}
}

/*
int main()
{
	trashInit();
	int *a = malloc(sizeof(int));
	addToBin(a);
	int *b = malloc(sizeof(int));
	addToBin(b);
	int *c = malloc(sizeof(int));
	addToBin(c);
	myFree(b);
	myFree(a);
	trashDestroy();
 	return 0;
}*/