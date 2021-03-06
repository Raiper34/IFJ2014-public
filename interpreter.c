/*                @Project: IFJ14
-----------------------------------------------------
  @Author: Marek Bielik   xbieli05@stud.fit.vutbr.cz
  @Author: Filip Gulán    xgulan00@stud.fit.vutbr.cz
  @Author: Filip Ježovica xjezov01@stud.fit.vutbr.cz
  @Author: Luboš Matuška  xmatus29@stud.fit.vutbr.cz
  @Author: Eduard Rybár   xrybar04@stud.fit.vutbr.cz
-----------------------------------------------------
*/


#include "header.h"
#include "astack.h"
#include "ial.h"
#include "instrlist.h"
#include "interpreter.h"
#include "garbage.h"
#include "strangen.h"
#include "buildin.h"


#define INIC 31111





tListInstrukcii INSTR_PASKA; // INSTRUKCNA PASKA

void* zarazka;

list *GLOBFRAME;
list *MASTERTAB; // pomocna tabulka pre priame dat typy (int, str,...)

astack FRAME; // zasobnik lokalnzch tabuliek
astack aS; // pomocny zasobnik adries pre interpreter

astack JMPSTACK; // zasobnik skokov // POZOR TOTO MUSI BYT FRONTA
astack paramSTACK; // parametre funkcii


void inic(void* aad, tStav stav)
{
	switch(stav)
	{
	case S_INTEGER:
	{
		if (   *( *(int **) aad ) == INIC)
	    {
	        // neinicializovana premenna
	        fprintf(stderr, "7: Neinicializovane!\n" );
	        trashDestroy(7);
	    }
	    break;
	}

	case S_DOUBLE:
	{
		if (   *( *(float **)  aad) == INIC)
	    {
	        // neinicializovana premenna
	        fprintf(stderr, "7: Neinicializovane!\n" );
	        trashDestroy(7);
	    }
	    break;
	}

	case S_RETEZEC:
	{
		if (  (strcmp(  (*(void **) aad) , "f!%4")) == 0    )
        {
            // neinicializovana premenna
            fprintf(stderr, "7: Neinicializovane!\n" );
            trashDestroy(7);
        }
        break;
	}

	default:
	{
		break;
	}

	}


}

void inter()    //AKCIA, KDE,int *PRVA,int *DRUHA//
{

    astack_init(&aS);
    myaPush(&aS, zarazka); // nahadzuje sa hlavna zarazka

    astack_init(&JMPSTACK); // POZOR FRONTA mA BYT
    myaPush(&JMPSTACK, zarazka);

    astack_init(&paramSTACK);

    

    tStav TIP;
    tInstrukcia *Instr; // lokalna instrukcia
    InstrStart(&INSTR_PASKA); // aktivovat instrukcnu pasku


    while(INSTR_PASKA.Aktivna != NULL) // dokedy neprideme na koniec INSTRUKCNEJ PASKY...
    {
        Instr = DajInstr(&INSTR_PASKA);

        //printf("INSTR_<<%d>>\n",  Instr->AKCIA );

        switch(Instr->AKCIA)
        {

        case I_NICNEROBA:
        {


            break;
        }

//-----------------------------ak pride int,double,boolean,string..-----------------------------------//
        case I_PREC:
        {
            TIP = *(tStav *)(Instr->ADDR_DRUHA);				// na tejto adrese musi byt napr. S_INTEGER
            
            /*hash_insert_it (MASTERTAB, Instr->ADDR_KDE, TIP );  //

            list_element prvok;

            prvok = (list_element)(hash_adress(MASTERTAB, Instr->ADDR_KDE)); // ID kluc

            (prvok)->ref = Instr->ADDR_PRVA;

            myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
            */

            //------------------------- UNIKATNY KLUC DO TABULKY MASTER-------------------------------------//
               strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, TIP );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = Instr->ADDR_PRVA;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
            //---------------------- UNIKATNY KLUC DO TABULKY MASTER--------------------------------------------/


            break;
        }

//------------------------------------ak pride IDENTIFIKATOR------------------------------------------//
        case I_IDENT:
        {
            list *TOPFRAME;
            TOPFRAME = myaTop(&FRAME);    // pozrieme sa na najvrchnejsi FRAME //

            list_element prvok;
            prvok = (list_element)(hash_adress(TOPFRAME, Instr->ADDR_PRVA));

            if (prvok == NULL) // hladame v GLOBAL
            {
                prvok = (list_element)(hash_adress(GLOBFRAME, Instr->ADDR_PRVA));
            }

            TIP = (*prvok).type;
            if (TIP == S_KLIC_BOOLEAN)
            {
                TIP = S_BOOLEAN;
            }

            myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat

            break;
        }


//----------------------------PRIRADENIE----------------------------------------------------------------//
        case I_PRIRAD:
        {
            if (TIP == S_INTEGER)
            {

                int pomoc1 = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS), S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                *( *(int **) (myaTop(&aS))) = pomoc1 ;
                myaPop(&aS);
            }
            else if (TIP == S_DOUBLE)
            {
                float pomoc1 = *(*(float**)(myaTop(&aS)));
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                *(*(float**)(myaTop(&aS))) = pomoc1 ;
                myaPop(&aS);
            }
            else if (TIP == S_RETEZEC)
            {
                void* pomAddr1; // to co vytiahneme zo zasobnika ako NAJVRCHNEJSIE

                pomAddr1 = (*(void **)(myaTop(&aS)));  // IDENTIFIKATOR (CEZ TABULKU SYMBOLOV)
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS); // mozme zahodit z zasobniku


                // berieme zo zasobniku dalsiu ADRESU - KDE  to ulozit (adresacia cez TABULKU SYMBOLOV)
                void* pomAddr2 = (*(void **)(myaTop(&aS)));
                int dlzka = strlen(((char**)pomAddr1));


                void *pom = myrealloc( pomAddr2 ,( ((sizeof(char))*dlzka)+2 ) );     //..realok
                void **kk = (myaTop(&aS));
                *kk = pom; // ked sa nahodu zmeni adresa po alokacii tak ulozime...

                strcpy( ((char*) pom) , ((char*)pomAddr1)  );

                myaPop(&aS); // pop 2 adresa


            }
            else if ( (TIP == S_KLIC_TRUE) || (TIP == S_KLIC_FALSE) || (TIP == S_BOOLEAN) )
            {
                bool pomoc1 = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);

                *( *(bool **) (myaTop(&aS))) = pomoc1 ;
                myaPop(&aS);
            }

            break;
        }



//-----------------------------------------------ALLOC pripady-----------------------------------------//
        case I_ALLOC_INT:
        {
            (*(void **)(myaTop(&aS))) = mymalloc(sizeof(int));

            *( *(int **) (myaTop(&aS))) =  INIC;

            myaPop(&aS);

            break;
        }


        case I_ALLOC_DOU:
        {
            (*(void **)(myaTop(&aS))) = mymalloc(sizeof(float));

            *( *(float **) (myaTop(&aS))) = INIC ;

            myaPop(&aS);

            break;
        }



        case I_ALLOC_BOO:
        {
            (*(void **)(myaTop(&aS))) = mymalloc(sizeof(bool));

            myaPop(&aS);

            break;
        }

        case I_ALLOC_STR:
        {
            (*(void **)(myaTop(&aS))) = mymalloc(sizeof(char)*4);

            char* ppp = ( *(char **) (myaTop(&aS))) ;
            ppp[0] = 'f';
            ppp[1] = '!';
            ppp[2] = '%';
            ppp[3] = '4';

            myaPop(&aS);

            break;
        }


//-----------------------------------------------READ-----------------------------------------------//

        case I_READ:
        {
            list *TOPFRAME;
            TOPFRAME = myaTop(&FRAME);

            list_element prvok;
            prvok = (list_element)(hash_adress(TOPFRAME, Instr->ADDR_PRVA));

            if (prvok == NULL) // hladame v GLOBAL
            {
                prvok = (list_element)(hash_adress(GLOBFRAME, Instr->ADDR_PRVA));
            }

            switch ((*prvok).type)
            {
            case S_INTEGER:
            {
                int LOL = scanf("%d", (prvok)->ref );

                if (LOL == 0)
                {
                	fprintf(stderr, "6: Behova chyba pri nacitani ciselnej hodnoty zo vstupu!\n" );
                	trashDestroy(6);
                }

                break;
            }

            case S_DOUBLE:
            {
                int LOL = scanf("%g", (prvok)->ref );

                if (LOL == 0)
                {
                	fprintf(stderr, "6: Behova chyba pri nacitani ciselnej hodnoty zo vstupu!\n" );
                	trashDestroy(6);
                }

                break;
            }

            case S_RETEZEC:
            {

                char znak;
                int dlzkastringu = 0;
                while((znak=getchar())!= '\n' )
                {
                    (prvok)->ref = myrealloc( (prvok)->ref , (sizeof(char)*dlzkastringu+2));

                    //  OK  *(char *) &((prvok)->ref)[dlzkastringu] = znak;  magia takto to ide :D
                    *(char *) &((prvok)->ref)[dlzkastringu] = znak;
                    dlzkastringu++;
                }

                *(char *) &((prvok)->ref)[dlzkastringu+1] = '\0';  // pridame znak ukoncenia retazca

                break;
            }

            default:
            {
                //chybka
                break;
            }

            }

            break;
        }



//------------------------------------------------WRITE -----------------------------------------------//
        case I_WRITE_IDE:
        {
            // treba zistit akeho je tipu //  // PRIDALI SME POP uvidime :D
            switch(TIP)
            {
            case S_INTEGER:
            {
            	inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie

                printf("%d", *( *(int **) (myaTop(&aS))) );
                myPop(&aS);
                break;
            }

            case S_DOUBLE:
            {
            	inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie

                printf("%g", *( *(float **) (myaTop(&aS)))  );
                myPop(&aS);
                break;
            }

            case S_RETEZEC:
            {
            	inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie

                printf("%s", ( *(char **) (myaTop(&aS))) );
                myPop(&aS);
                break;
            }

            case S_KLIC_FALSE:
            case S_KLIC_TRUE:
            case S_BOOLEAN:
            {
            	if ( *( *(bool **) (myaTop(&aS))) )
            	{
            		printf("TRUE");
            	}
            	else
            	{
            		printf("FALSE");
            	}
                //printf("%d", *( *(bool **) (myaTop(&aS)))  );
                myPop(&aS);
                break;
            }

            default:
            {
                break;
            }

            }

            break;
        }



        case I_WRITE_INT:
        {
            printf("%d",   *( *(int **) (myaTop(&aS)))   );
            myPop(&aS);

            break;
        }

        case I_WRITE_DOU:
        {
            printf("%g", *( *(float **) (myaTop(&aS))) );
            myPop(&aS);

            break;
        }

        case I_WRITE_STR:
        {
            printf("%s", ( *(char **) (myaTop(&aS)))    );
            myPop(&aS);

            break;
        }

        case I_WRITE_BOO:
        {
        	if ( *( *(bool **) (myaTop(&aS))) )
            {
            	printf("TRUE");
            }
            else
            {
            	printf("FALSE");
            }
            //printf("%d", *( *(bool **) (myaTop(&aS)))  );
            myPop(&aS);

            break;
        }


//------------------------------------------ +,-,*, / -------------------------------------------------//
        case I_PLUS:
        {

            if (TIP == S_INTEGER)  // adresa vs cislo toto treba opravit
            {
                int a = *( *(int **) (myaTop(&aS)))  ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);


                int b = *( *(int **) (myaTop(&aS)))  ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int c_integer = mymalloc(sizeof(int));

                *(int*)c_integer = b + a;   // spocitaju sa hodnoty a priradia sa do medzi vysledku


                //-----------INTEGER-------UNIKATNY KLUC DO TABULKY MASTER------------------------------------------------//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_INTEGER );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_integer ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //-----------INTEGER-------UNIKATNY KLUC DO TABULKY MASTER----------------------------------------------//



            }
            else if (TIP == S_DOUBLE)
            {
                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                int c_double = mymalloc(sizeof(float));

                *(float*)c_double = b + a;
                
                //-----------DOUBLE-------UNIKATNY KLUC DO TABULKY MASTER------------------------------------------------//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_DOUBLE );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_double ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //-----------DOUBLE-------UNIKATNY KLUC DO TABULKY MASTER----------------------------------------------//

            }
            else if (TIP == S_RETEZEC)
            {
                void* pomAddr1;
                void* pomAddr2;
                void* c_string = mymalloc(sizeof(char));

                pomAddr1 = (*(void **)(myaTop(&aS))) ;
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                pomAddr2 = (*(void **)(myaTop(&aS))) ;
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);
                //int dlzka = (  (strlen((*(char**)pomAddr1))) +  (strlen((*(char**)pomAddr2)))    );

// SKONTROLOVAT CI DAVA DOBRU DLZKU...-------------------------------------------------------------------------!!!!!-----

                int dlzka = strlen( pomAddr2 ) + strlen( pomAddr1 );
                void* pomAddr3 = mymalloc(((sizeof(char))*dlzka)+1);

                strcpy(  pomAddr3, ((char**)pomAddr2)  );

                c_string = myrealloc(   c_string  , ( ((sizeof(char))*dlzka)+2 ) );

                strcpy(  c_string, ((char**)pomAddr3)  );
                strcat(  c_string, ((char**)pomAddr1)  );

                
                //-----------STRING-------UNIKATNY KLUC DO TABULKY MASTER------------------------------------------------//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_DOUBLE );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_string ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //-----------STRING-------UNIKATNY KLUC DO TABULKY MASTER----------------------------------------------//


            }


            break;
        }


//-------------------------------------------------MINUS-----------------------------------------------//
        case I_MINUS:
        {
            if (TIP == S_INTEGER)  // adresa vs cislo toto treba opravit
            {
                int a = *( *(int **) (myaTop(&aS)))  ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int b = *( *(int **) (myaTop(&aS)))  ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int c_integer = mymalloc(sizeof(int));

                *(int*)c_integer = b - a;   // spocitaju sa hodnoty a priradia sa do medzi vysledku


                //-----------INTEGER-------UNIKATNY KLUC DO TABULKY MASTER------------------------------------------------//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_INTEGER );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_integer ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //-----------INTEGER-------UNIKATNY KLUC DO TABULKY MASTER----------------------------------------------//

            }
            else if (TIP == S_DOUBLE)
            {
                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                int c_double = mymalloc(sizeof(float));

                *(float*)c_double = b - a;


                //-----------DOUBLE-------UNIKATNY KLUC DO TABULKY MASTER------------------------------------------------//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_DOUBLE );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_double ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //-----------DOUBLE-------UNIKATNY KLUC DO TABULKY MASTER----------------------------------------------//


            }

            break;
        }


//---------------------------------------------KRAT-------------------------------------------------------//
        case I_KRAT:
        {
            if (TIP == S_INTEGER)  // adresa vs cislo toto treba opravit
            {
                int a = *( *(int **) (myaTop(&aS)))  ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int b = *( *(int **) (myaTop(&aS)))  ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int c_integer = mymalloc(sizeof(int));

                *(int*)c_integer = b * a;   // spocitaju sa hodnoty a priradia sa do medzi vysledku


                //-----------INTEGER-------UNIKATNY KLUC DO TABULKY MASTER------------------------------------------------//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_INTEGER );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_integer ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //-----------INTEGER-------UNIKATNY KLUC DO TABULKY MASTER----------------------------------------------//


            }
            else if (TIP == S_DOUBLE)
            {
                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                int c_double = mymalloc(sizeof(float));

                *(float*)c_double = b * a;


                //-----------DOUBLE-------UNIKATNY KLUC DO TABULKY MASTER------------------------------------------------//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_DOUBLE );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_double ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //-----------DOUBLE-------UNIKATNY KLUC DO TABULKY MASTER----------------------------------------------//

            }

            break;
        }


//---------------------------------------------------DELENO----------------------------------------------//
        case I_DELENO:
        {
            if (TIP == S_INTEGER)  // adresa vs cislo toto treba opravit
            {
                int a = *( *(int **) (myaTop(&aS)))  ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                if (a == 0)
                {
                    //DELENIE NULOV
                    fprintf(stderr, "8: Behova chyba. Snazis sa delit NULOU ! \n");
                    trashDestroy(behova_chyba_deleni_nulou); // chyba 8
                }
                
                int b = *( *(int **) (myaTop(&aS)))  ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int c_integer = mymalloc(sizeof(int));

                *(int*)c_integer = b / a;   // spocitaju sa hodnoty a priradia sa do medzi vysledku



                //-----------INTEGER-------UNIKATNY KLUC DO TABULKY MASTER------------------------------------------------//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_INTEGER );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_integer ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //-----------INTEGER-------UNIKATNY KLUC DO TABULKY MASTER----------------------------------------------//


            }
            else if (TIP == S_DOUBLE)
            {
                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                if (a == 0.0)
                {
                    //DELENIE NULOV
                    fprintf(stderr, "8: Behova chyba. Snazis sa delit NULOU ! \n");
                    trashDestroy(behova_chyba_deleni_nulou); // chyba 8
                }

                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                int c_double = mymalloc(sizeof(float));

                *(float*)c_double = b / a;


                //-----------DOUBLE-------UNIKATNY KLUC DO TABULKY MASTER------------------------------------------------//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_DOUBLE );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_double ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //-----------DOUBLE-------UNIKATNY KLUC DO TABULKY MASTER----------------------------------------------//


            }

            break;
        }




//---------------------------------- POROVNAVACIE INSTRUKCIE  dole ----------------------------//

        case I_ROVNO:
        {
            void* c_bool = mymalloc(sizeof(bool));


            //ok
            if (TIP == S_INTEGER) /*int = int*/
            {
                int a = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int b = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);
                if (a == b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//



            }
            if (TIP == S_DOUBLE) /* real = real */
            {
                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);
                if (a == b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//

            }
            if (TIP == S_RETEZEC) /* string = string */
            {
                char* pomAddr1 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                char* pomAddr2 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                if (strcmp (pomAddr1, pomAddr2))
                {
                    *(bool*)c_bool = false;
                }
                else
                {
                    *(bool*)c_bool = true;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }

            if ( (TIP == S_BOOLEAN) || (TIP == S_KLIC_TRUE) || (TIP == S_KLIC_FALSE) ) /* bool = bool */
            {
                bool a = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                bool b = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                if (a == b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//

            }



            break;
        }



        case I_NEROVNO:
        {
            void* c_bool = mymalloc(sizeof(bool));

            //ok
            if (TIP == S_INTEGER) /*int <> int*/
            {
                int a = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int b = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);
                if (a != b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_DOUBLE) /* real <> real */
            {
                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                if (a != b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_RETEZEC) /* string <> string */
            {
                char* pomAddr1 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                char* pomAddr2 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                if ((strcmp (pomAddr1, pomAddr2)))
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }



            if ( (TIP == S_BOOLEAN) || (TIP == S_KLIC_TRUE) || (TIP == S_KLIC_FALSE) ) /* bool != bool */
            {
                bool a = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                bool b = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                if (a != b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }


                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//

            }



            break;
        }



        case I_VETSI:
        {
            void* c_bool = mymalloc(sizeof(bool));


            //ok
            if (TIP == S_INTEGER) /*int > int*/
            {
                int b = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int a = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);
                if (a > b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_DOUBLE) /* real > real */
            {
                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                if (a > b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_RETEZEC) /* string > string */
            {
                char* pomAddr1 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                char* pomAddr2 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                if ((strcmp (pomAddr2, pomAddr1)) > 0)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }


            if ( (TIP == S_BOOLEAN) || (TIP == S_KLIC_TRUE) || (TIP == S_KLIC_FALSE) ) /* bool > bool */
            {
                bool b = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                bool a = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                if (a > b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//

            }


            break;
        }



        case I_MENSI:
        {
            void* c_bool = mymalloc(sizeof(bool));


            //ok
            if (TIP == S_INTEGER) /*int < int*/
            {
                int b = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int a = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                if (a < b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_DOUBLE) /* real < real */
            {
                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                if (a < b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_RETEZEC) /* string < string */
            {
                char* pomAddr1 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                char* pomAddr2 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                if ((strcmp (pomAddr2, pomAddr1)) < 0)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }


            if ( (TIP == S_BOOLEAN) || (TIP == S_KLIC_TRUE) || (TIP == S_KLIC_FALSE) ) /* bool < bool */
            {
                bool b = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                bool a = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                if (a < b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//

            }


            break;
        }



        case I_VETSIROVNO:
        {
            void* c_bool = mymalloc(sizeof(bool));


            //ok
            if (TIP == S_INTEGER) /*int >= int*/
            {
                int b = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int a = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                if (a >= b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_DOUBLE) /* real >= real */
            {
                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                if (a >= b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_RETEZEC) /* string >= string */
            {
                char* pomAddr1 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                char* pomAddr2 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                if ((strcmp (pomAddr2, pomAddr1)) >= 0)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }


            if ( (TIP == S_BOOLEAN) || (TIP == S_KLIC_TRUE) || (TIP == S_KLIC_FALSE) ) /* bool >= bool */
            {
                bool b = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                bool a = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                if (a >= b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//

            }



            break;
        }

        case I_MENSIROVNO:
        {
            void* c_bool = mymalloc(sizeof(bool));

            
            //ok
            if (TIP == S_INTEGER) /*int <= int*/
            {
                int b = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                int a = *( *(int **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_INTEGER); // overenie inicializacie
                myaPop(&aS);

                if (a <= b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_DOUBLE) /* real <= real */
            {
                float b = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                float a = *( *(float **) (myaTop(&aS))) ;
                inic(myaTop(&aS) , S_DOUBLE); // overenie inicializacie
                myaPop(&aS);

                if (a <= b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }
            if (TIP == S_RETEZEC) /* string <= string */
            {
                char* pomAddr1 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                char* pomAddr2 = ( *(char **) (myaTop(&aS)));
                inic(myaTop(&aS) , S_RETEZEC); // overenie inicializacie
                myaPop(&aS);

                if ((strcmp (pomAddr2, pomAddr1)) <= 0)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//


            }


            if ( (TIP == S_BOOLEAN) || (TIP == S_KLIC_TRUE) || (TIP == S_KLIC_FALSE) ) /* bool <= bool */
            {
                bool b = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                bool a = *( *(bool **) (myaTop(&aS))) ;
                myaPop(&aS);
                if (a <= b)
                {
                    *(bool*)c_bool = true;
                }
                else
                {
                    *(bool*)c_bool = false;
                }



                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//
                strningRandGen();

                hash_insert_it (MASTERTAB, UNIKAT, S_BOOLEAN );

                list_element prvok;

                prvok = (list_element)(hash_adress(MASTERTAB, UNIKAT  )); // ID kluc
                (prvok)->ref = c_bool ;
                myaPush(&aS, &(prvok)->ref); // vlozime na zasobnik adresu chlieviku v ktorom je adresa na danu polozku dat
                //***************** UNIKATNY KLUC DO TABULKY MASTER*************************************************//

            }



            break;
        }




//--------------------------------------------INSTRUKCIE SKOKOV------------------TEST VER--------LEN FUNKCIE---------//

        case I_JMPF_KEY:
        {
            // vstavane funkcie
            
            // ---- Vstavana funkcia --- LENGTH--------------
            if ( strcmp( Instr->ADDR_PRVA, "length" )  == 0 )
            {
            	list *TOPFRAME;
            	TOPFRAME = myaTop(&FRAME);

            	list_element prvok;
            	list_element navrat;
                

            	navrat = (list_element)(hash_adress(TOPFRAME, Instr->ADDR_PRVA )); // navratova hodnota

                // toto netreba
                //prvok = (list_element)(hash_adress(TOPFRAME, "s")); // prvy parameter funkcie (tu musi prist STRING)
                //((prvok)->ref )  = mymalloc(sizeof(char)+1);
                
                ((navrat)->ref ) = mymalloc(sizeof(int));
                
                myaPush(&aS, &(navrat)->ref);
//--------------------------------------------------------------------------------------------

//PARAM STACK == TO CO CHCEME SKOPIROVAT tu su DATA

                    //( *(char **) (myaTop(&aS))) =  ( *(char **) (myaTop(&paramSTACK)))  ; // priradime do prvok->ref

                    //to co kopirujeme
                    void* pomAddr1; 
                    pomAddr1 = (*(void **) (myaTop(&paramSTACK)));
                    int dlzka = strlen(((char**)pomAddr1));



            myaPop(&paramSTACK); // odstranime z pomocneho
            //myaPop(&aS); // odstranime z klasik


//-----------------------------------------------------------------------------------------------

                (*(int **)  (navrat)->ref)   =  lenght(  pomAddr1   );  // pom je char



            }
            else if ( strcmp( Instr->ADDR_PRVA, "copy" )  == 0 )
            {
            	// ---- Vstavana funkcia --- COPY--------------
            	list *TOPFRAME;
            	TOPFRAME = myaTop(&FRAME);

            	list_element prvok;
            	list_element navrat;
                

            	navrat = (list_element)(hash_adress(TOPFRAME, Instr->ADDR_PRVA )); // navratova hodnota

                //toto netreba asi .. racej to tu neham este
                //prvok = (list_element)(hash_adress(TOPFRAME, "s")); // prvy parameter funkcie (tu musi prist STRING)
                //((prvok)->ref )  = mymalloc(sizeof(char)+1);

                //podretazec
                    ((navrat)->ref ) = mymalloc((sizeof(char)) + 2);
                    myaPush(&aS, &(navrat)->ref);
//--------------------------------------------------------------------------------------------

                    //to co kopirujeme
                    void* pomAddr1; 
                    pomAddr1 = (*(void **) (myaTop(&paramSTACK))); // 1 param uz priamo DATA
                    int dlzka = strlen(((char**)pomAddr1));

                    myaPop(&paramSTACK); // odstranime z pomocneho teraz tam mame 2 param

                    
                    int pom2Zac =  *( *(int **) (myaTop(&paramSTACK)))  ; // 2 param  ZACIATOK PODRETAZCA
                    //printf(">>>2param<<%d>>\n", pom2Zac);

                    myaPop(&paramSTACK); // odstranime z pomocneho teraz tam mame 3 param

 
                    int pom3Dlzka = *( *(int **) (myaTop(&paramSTACK)))  ; // 3 param DLZKA PODRETAZCA 
                    //printf(">>>3param<<%d>>\n", pom3Dlzka);

                    myaPop(&paramSTACK); // odstranime z pomocneho
                    //ok parametre prebrane

                    //podretazec
                    void *pom = myrealloc( ((navrat)->ref ) ,( ((sizeof(char))*pom3Dlzka)+2 ) );     //..realok
                    void **kk = &((navrat)->ref );
                    *kk = pom; // ked sa nahodu zmeni adresa po alokacii tak ulozime...
                    
//-----------------------------------------------------------------------------------------------
                    copy(pomAddr1, pom2Zac, pom3Dlzka, (navrat)->ref  );  // void funkcia

                    // ok hotovo

            }
            else if ( strcmp( Instr->ADDR_PRVA, "find" )  == 0 )
            {
            	// ---- Vstavana funkcia --- FIND--------------
            	list *TOPFRAME;
            	TOPFRAME = myaTop(&FRAME);

            	list_element prvok;
            	list_element navrat;
                

            	navrat = (list_element)(hash_adress(TOPFRAME, Instr->ADDR_PRVA )); // navratova hodnota

                //toto netreba asi .. racej to tu neham este
                //prvok = (list_element)(hash_adress(TOPFRAME, "s")); // prvy parameter funkcie (tu musi prist STRING)
                //((prvok)->ref )  = mymalloc(sizeof(char)+1);

                ((navrat)->ref ) = mymalloc(sizeof(int));
                

                myaPush(&aS, &(navrat)->ref);
//-----------------------------------------------------------------------------------------------
 

                void* pomAddr1; 
                pomAddr1 = (*(void **) (myaTop(&paramSTACK))); // 1 param uz priamo DATA

                myaPop(&paramSTACK); // odstranime z pomocneho teraz tam mame 2 param


                void* pomAddr2; 
                pomAddr2 = (*(void **) (myaTop(&paramSTACK))); // 2 param uz priamo DATA

                myaPop(&paramSTACK); // odstranime z pomocneho


            	(*(int **)  (navrat)->ref)  =  find(pomAddr1, pomAddr2);  //find(char *T, char *P);



            }
            else if ( strcmp( Instr->ADDR_PRVA, "sort" )  == 0 )
            {
            	// ---- Vstavana funkcia --- SORT--------------
            	list *TOPFRAME;
            	TOPFRAME = myaTop(&FRAME);

            	list_element prvok;
            	list_element navrat;
                

            	navrat = (list_element)(hash_adress(TOPFRAME, Instr->ADDR_PRVA )); // navratova hodnota
                 
                //toto netreba asi .. racej to tu neham este
                //prvok = (list_element)(hash_adress(TOPFRAME, "s")); // prvy parameter funkcie (tu musi prist STRING)
                //((prvok)->ref )  = mymalloc(sizeof(char)+1);

                ((navrat)->ref ) = mymalloc(sizeof(int));
                

                myaPush(&aS, &(navrat)->ref);
//--------------------------------------------------------------------------------------------

//PARAM STACK == TO CO CHCEME SKOPIROVAT tu su DATA

                    //to co kopirujeme
                    void* pomAddr1; 
                    pomAddr1 = (*(void **) (myaTop(&paramSTACK)));
                    int dlzka = strlen(((char**)pomAddr1));


                    // KAM kopirujeme
                    void* pomAddr2 = (*(void **)(myaTop(&aS)));  // tam je navrat->ref


                    void *pom = myrealloc( pomAddr2 ,( ((sizeof(char))*dlzka)+2 ) );     //..realok
                    void **kk = (myaTop(&aS));
                    *kk = pom; // ked sa nahodu zmeni adresa po alokacii tak ulozime...

                    strcpy( ((char*) pom) , ((char*)pomAddr1)  );


            myaPop(&paramSTACK); // odstranime z pomocneho
            //myaPop(&aS); // odstranime z klasik

//ok uz mame vsetko potrebne v navratovom ... teraz to priamo v tom zosortujeme..
//-----------------------------------------------------------------------------------------------
                sort(  ((void **) (navrat)->ref) , 0, dlzka-1  ); // toto ho zoSORTUJE
            }
            else
            {
            			// UZIVATELSKA FUNKCIA
            
                        //printf("INTER_SKOC podla kluca Z GLOB TAB!\n");
                        //skoc podla KLUCA z GLOB TAB == tam kde zacina urcita funkcia
                        list_element prvok;
                        prvok = (list_element)(hash_adress(GLOBFRAME, Instr->ADDR_PRVA));
            
                        InstrGoto(&INSTR_PASKA, (prvok)->start  );
                        //continue;
            }




            break;
        }


        case I_JMPF_KEY_S:
        {
            //ok
           // printf("INTER_uloz aktualnu adresu do GLOB TAB! : navestie funkcie \n");
            // uloz adresu KDE sa skoci  do GLOB TAB... cize navestie funkcie

            list_element prvok;
            prvok = (list_element)(hash_adress(GLOBFRAME, Instr->ADDR_KDE));
            void* pomad;
            pomad = InstrDajPoz(&INSTR_PASKA);
            // &(prvok)->ref

            ((prvok)->start) = pomad;


            break;
        }


        case I_JMP_S:
        {
            //printf("INTER_uloz aktualnu adresu do zasobnika skokov! : navratova adresa po skonceni fcie \n");
            //uloz si AKTUALNU poziciu do zasobniku skokov
            myaPush(&JMPSTACK, InstrDajPoz(&INSTR_PASKA));

            break;
        }

        case I_JMP_BACK:
        {
            //printf("INTER_skoc na adresu zo zasobnika skokov! : konci funkcia skoc odkial si zacal \n");

            // skok na ADRESU co je ulozena v zasobniku SKOKOV
            InstrGoto(&INSTR_PASKA, myaTop(&JMPSTACK));
            myaPop(&JMPSTACK);
            //poskoc na dalsiu instr
            InstrDalsia(&INSTR_PASKA);

            break;
        }


        case I_FUN_PRIRAD_PARAM:
        {

            //printf("INTER_napln parametre funkcie! \n");
            // ber co mas na pomocnom a prirad do toho co je na aS stacku
            

            switch(   *(tStav *)(Instr->ADDR_DRUHA)   )
            {
                case S_INTEGER:
                {
                	inic(myaTop(&paramSTACK) , S_INTEGER); // overenie inicializacie
                    *( *(int **) (myaTop(&aS))) =  *( *(int **) (myaTop(&paramSTACK)))  ; // priradime do prvok->ref

                    break;
                }

                case S_DOUBLE:
                {
                	inic(myaTop(&paramSTACK) , S_DOUBLE); // overenie inicializacie
                    *( *(float **) (myaTop(&aS))) =  *( *(float **) (myaTop(&paramSTACK)))  ; // priradime do prvok->ref

                    break;
                }

                case S_BOOLEAN:
                case S_KLIC_FALSE:
                case S_KLIC_TRUE:
                {
                    *( *(bool **) (myaTop(&aS))) =  *( *(bool **) (myaTop(&paramSTACK)))  ; // priradime do prvok->ref
                    
                    break;
                }

                case S_RETEZEC:
                {
                    //PARAM STACK == TO CO CHCEME SKOPIROVAT tu su DATA

                    //( *(char **) (myaTop(&aS))) =  ( *(char **) (myaTop(&paramSTACK)))  ; // priradime do prvok->ref

                    //to co kopirujeme
                    inic(myaTop(&paramSTACK) , S_RETEZEC); // overenie inicializacie
                    void* pomAddr1; 
                    pomAddr1 = (*(void **) (myaTop(&paramSTACK)));
                    int dlzka = strlen(((char**)pomAddr1));

                    //printf("SZZZZZZ>>%s<<<dlzka je >%d<\n",  pomAddr1 , dlzka);


                    // KAM kopirujeme
                    void* pomAddr2 = (*(void **)(myaTop(&aS)));

                    


                    void *pom = myrealloc( pomAddr2 ,( ((sizeof(char))*dlzka)+2 ) );     //..realok
                    void **kk = (myaTop(&aS));
                    *kk = pom; // ked sa nahodu zmeni adresa po alokacii tak ulozime...

                    strcpy( ((char*) pom) , ((char*)pomAddr1)  );

                    


                    break;
                }



                default:
                    {
                        break;
                    }
            }

            myaPop(&paramSTACK); // odstranime z pomocneho
            myaPop(&aS); // odstranime z klasik

            break;
        }



        case I_RUNFUN_PARAM:
        {
            //printf("INTER_pred spustenim fcie uloz parametre na zasobnik \n");
            // prehadz mi parametre funkcie na parametrovy zasobnik!
            while(myaTop(&aS) != zarazka)
            {

                myaPush(&paramSTACK, myaTop(&aS) )  ;

                myaPop(&aS);
            }
            myaPop(&aS); // vyhod zarazku


            break;
        }


        case I_ZARAZKA:
        {
            myaPush(&aS, zarazka);
            break;
        }


        case I_RUNFUN_COPY:
        {
            //printf("INTER_skopiruj TABULKU podla kluca daj ako novy FRAME! \n");
            //zkopiruje tabulku podla kluca..
            list *NOVATABULKA;

            list_element prvok;
            prvok = (list_element)(hash_adress(GLOBFRAME, Instr->ADDR_PRVA));

            NOVATABULKA = copyhash(  (prvok)->ref  ); // skopiruje tabulku

            myaPush(&FRAME, NOVATABULKA);
            //myaPush(&aS, zarazka);
            break;
        }



        case I_FRAME_DEL:
        {
            //printf("INTER_koniec fcie delete FRAME \n");
            // vyhodime FRAME koniec funkcie
            myaPop(&FRAME);

            break;
        }


//----------------------potialto su INSTRUKCIE NA FUNKCIE --------------------------------------------------//

        case I_JMP:
        {

            //DIRECT JUMP

            //NaplnInstr(I_PODM_JMP, myaSecTop(&IFJMP), NULL, NULL);

            InstrGoto(&INSTR_PASKA,   *(void**) (Instr->ADDR_KDE)   );

            continue;
            //InstrDalsia(&INSTR_PASKA);






            break;
        }

        case I_PODM_JMP:
        {

            if (   !(*( *(bool **) (myaTop(&aS))))     )
            {

                InstrGoto(&INSTR_PASKA,   *(void**) (Instr->ADDR_KDE)    );
                myaPop(&aS); // toto som pridal
                continue;
                //InstrDalsia(&INSTR_PASKA);



            }
            //else neskace
            myaPop(&aS); // toto som pridal
            break;
        }


        default:
        {
            break;
        }


        }

        InstrDalsia(&INSTR_PASKA);
    }

    // tuto bude dealokacia pomocnych premenych

    return ; // VSETKO OK
}
