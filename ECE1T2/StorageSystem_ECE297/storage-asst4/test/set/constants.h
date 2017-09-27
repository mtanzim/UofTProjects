/*Constant Declarations*/

#define SERVEREXEC	"./server"	// Server executable file.
#define SERVEROUT	"server.out"	// File where the server's output is stored.
#define SERVEROUT_MODE	0666		// Permissions of the server ouptut file.
#define ONETABLE_CONF	"fourtables.conf"// Server configuration file with one table.
#define TESTTIMEOUT	10		// How long to wait for each test to run.

// These settings should correspond to what's in the config file.
#define SERVERHOST	"localhost"	// The hostname where the server is running.
#define SERVERPORT	8888		// The port where the server is running.

#define TABLE1NAME	"subwayLines"		// The first table to use.
#define TABLE2NAME	"cities"		// The second table to use.
#define TABLE3NAME	"cars"
#define TABLE4NAME	"students"
#define TABLENAME_BLANK		""
#define TABLENAME_NOTEXIST	"rubbish"
#define TABLENAME_TOOLONG	"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"

#define KEY		"somekey"
#define KEY2		"somekey2"
#define KEY_BLANK	""
#define KEY_NOTEXIST	"rubbish"
#define KEY_TOOLONG	"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"

#define VALUE_NOTEXIST	"name rubbish, stops rubbish"
#define VALUE_INVALID	"iiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"
#define VALUE_INVALID2	"rubbish"
/* Anything above this line should not be touched.

##################  Assume all other unmentioned params are correct.
1. Spaces and commas
	1.1. all commas present, 0-all spaces		VALID
	1.2. no commas present, 0-all spaces		INVALID
	1.3. 1.2 with some commas			INVALID with exceptions

1o2.: 1. with 2 tables (not every single value in 1. will be used)
1o3.: 1. with 3 tables (not every single value in 1. will be used)
1o4.: 1. with 4 tables (not every single value in 1. will be used)

2. Spelling
	2.1. 1 mistake
		2.1.1. table name
		2.1.2. column name
	2.2. 2 mistakes
		2.2.1. 2 table names
		2.2.2. 2 column names
(2.2.3 - 2.3.4 are redundant.)
		2.2.3. 1 table name, 1 column name
	2.3. 3 mistakes
		2.3.1. 3 table names
		2.3.2. 3 column names
		2.3.3. 1 table name, 2 column names
		2.3.4. 2 table names, 1 column name
3. Missing params
	3.1. 1-4missing table, table name
	3.2. 1-3 missing column parameter for each table
	3.3. Missing table name and column parameter

4. Data types
	4.1. subway table
		4.1.1.: name: float 
		4.1.2.: name: large positive int
		4.1.3.: name: large negative int
		4.1.4.: stops: string
		4.1.5.: stops: float
		4.1.6.: stops: -ve int
		4.1.7.: kilometres: -ve float
		4.1.8.: kilometres: + int
		4.1.9.: kilometres: - int
5. Character length
6. Order of params (redundant)

*/

/*1.1.1. all commas present, 0-all-extra spaces. All must produce VALID returns  */

#define VALUEsubway_111_0000	"name Bloor Danforth,stops 31,kilometres 26.2"	
#define VALUEsubway_111_0001	"name Bloor Danforth,stops 31, kilometres 26.2"
#define VALUEsubway_111_0010	"name Bloor Danforth,stops 31 ,kilometres 26.2"
#define VALUEsubway_111_0011	"name Bloor Danforth,stops 31 , kilometres 26.2"
#define VALUEsubway_111_0100	"name Bloor Danforth, stops 31,kilometres 26.2"
#define VALUEsubway_111_0101	"name Bloor Danforth, stops 31, kilometres 26.2"
#define VALUEsubway_111_0110	"name Bloor Danforth, stops 31 ,kilometres 26.2"
#define VALUEsubway_111_0111	"name Bloor Danforth, stops 31 , kilometres 26.2"
#define VALUEsubway_111_1000	"name Bloor Danforth ,stops 31,kilometres 26.2"
#define VALUEsubway_111_1001	"name Bloor Danforth ,stops 31, kilometres 26.2"
#define VALUEsubway_111_1010	"name Bloor Danforth ,stops 31 ,kilometres 26.2"
#define VALUEsubway_111_1011	"name Bloor Danforth ,stops 31 , kilometres 26.2"
#define VALUEsubway_111_1100	"name Bloor Danforth , stops 31,kilometres 26.2"
#define VALUEsubway_111_1101	"name Bloor Danforth , stops 31, kilometres 26.2"
#define VALUEsubway_111_1110	"name Bloor Danforth , stops 31 ,kilometres 26.2"
#define VALUEsubway_111_1111	"name Bloor Danforth , stops 31 , kilometres 26.2"

/*Extra spaces*/
#define VALUEsubway_111_5000	"name Bloor Danforth     ,stops 31,kilometres 26.2"
#define VALUEsubway_111_1211	"name Bloor Danforth ,  stops 31 , kilometres 26.2"
#define VALUEsubway_111_1131	"name Bloor Danforth , stops 31   , kilometres 26.2"
#define VALUEsubway_111_11112	"name Bloor Danforth , stops 31 , kilometres 26.2  "

#define VALUEcity_111_0000 	"lowTemperature -7.6,highTemperature +28.0,province Ontario"
#define VALUEcity_111_0001	"lowTemperature -7.6,highTemperature +28.0, province Ontario"
#define VALUEcity_111_0010 	"lowTemperature -7.6,highTemperature +28.0 ,province Ontario"
#define VALUEcity_111_0011 	"lowTemperature -7.6,highTemperature +28.0 , province Ontario"
#define VALUEcity_111_0100 	"lowTemperature -7.6, highTemperature +28.0,province Ontario"
#define VALUEcity_111_0101 	"lowTemperature -7.6, highTemperature +28.0, province Ontario"
#define VALUEcity_111_0110 	"lowTemperature -7.6, highTemperature +28.0 ,province Ontario"
#define VALUEcity_111_0111 	"lowTemperature -7.6, highTemperature +28.0 , province Ontario"
#define VALUEcity_111_1000 	"lowTemperature -7.6 ,highTemperature +28.0,province Ontario"
#define VALUEcity_111_1001 	"lowTemperature -7.6 ,highTemperature +28.0, province Ontario"
#define VALUEcity_111_1010 	"lowTemperature -7.6 ,highTemperature +28.0 ,province Ontario"
#define VALUEcity_111_1011 	"lowTemperature -7.6 ,highTemperature +28.0 , province Ontario"
#define VALUEcity_111_1100 	"lowTemperature -7.6 , highTemperature +28.0,province Ontario"
#define VALUEcity_111_1101 	"lowTemperature -7.6 , highTemperature +28.0, province Ontario"
#define VALUEcity_111_1110 	"lowTemperature -7.6 , highTemperature +28.0 ,province Ontario"
#define VALUEcity_111_1111	 "lowTemperature -7.6 , highTemperature +28.0 , province Ontario"

#define VALUEcars_111_00	"brand BMW,price 43210.98"
#define VALUEcars_111_01	"brand BMW, price 43210.98"
#define VALUEcars_111_10	"brand BMW ,price 43210.98"
#define VALUEcars_111_11	"brand BMW , price 43210.98"

#define VALUEstudents_111_00	"id 991234567,Grade 87.6"
#define VALUEstudents_111_01	"id 991234567, Grade 87.6"
#define VALUEstudents_111_10	"id 991234567 ,Grade 87.6"
#define VALUEstudents_111_11	"id 991234567 , Grade 87.6"


/*1.1.2. no commas present, 0-all spaces. All must produce INVALID returns  */
#define INVALUEsubway_112_00	"name Bloor Danforthstops 31kilometres 26.2"
#define INVALUEsubway_112_01	"name Bloor Danforthstops 31 kilometres 26.2"
#define INVALUEsubway_112_10	"name Bloor Danforth stops 31kilometres 26.2"
#define INVALUEsubway_112_11	"name Bloor Danforth stops 31 kilometres 26.2"

#define INVALUEcity_112_000 	"lowTemperature -7.6highTemperature +28.0province Ontario"
#define INVALUEcity_112_001 	"lowTemperature -7.6highTemperature +28.0 province Ontario"
#define INVALUEcity_112_010 	"lowTemperature -7.6highTemperature +28.0 province Ontario"
#define INVALUEcity_112_011 	"lowTemperature -7.6highTemperature +28.0  province Ontario"
#define INVALUEcity_112_100 	"lowTemperature -7.6 highTemperature +28.0province Ontario"
#define INVALUEcity_112_101 	"lowTemperature -7.6 highTemperature +28.0 province Ontario"
#define INVALUEcity_112_110 	"lowTemperature -7.6 highTemperature +28.0 province Ontario"
#define INVALUEcity_112_111 	"lowTemperature -7.6 highTemperature +28.0  province Ontario"

#define INVALUEcars_112_00	"brand BMWprice 43210.98"
#define INVALUEcars_112_01	"brand BMW price 43210.98"
#define INVALUEcars_112_10	"brand BMW price43210.98"
#define INVALUEcars_112_11	"brand BMW  price 43210.98"

#define INVALUEstudents_112_00	"id 991234567Grade 87.6"
#define INVALUEstudents_112_01	"id 991234567 Grade 87.6"
#define INVALUEstudents_112_11	"id 991234567  Grade 87.6"

/*1.1.3.: 1.1.2. with variable commas. All must produce INVALID returns */
#define INVALUEsubway_113_1_000	"name Bloor Danforth,stops 31kilometres 26.2"
#define INVALUEsubway_113_1_001	"name Bloor Danforth,stops 31 kilometres 26.2"
#define INVALUEsubway_113_1_011	"name Bloor Danforth,stops 31  kilometres 26.2"

#define INVALUEsubway_113_2_000	"name Bloor Danforthstops 31,kilometres 26.2"
#define INVALUEsubway_113_2_001	"name Bloor Danforthstops 31, kilometres 26.2"
#define INVALUEsubway_113_2_010	"name Bloor Danforthstops 31 ,kilometres 26.2"
#define INVALUEsubway_113_2_011	"name Bloor Danforthstops 31 , kilometres 26.2"
#define INVALUEsubway_113_2_020	"name Bloor Danforthstops 31  ,kilometres 26.2"
#define INVALUEsubway_113_2_120	"name Bloor Danforth stops 31  ,kilometres 26.2"
#define INVALUEsubway_113_2_121	"name Bloor Danforth stops 31  , kilometres 26.2"

#define INVALUEcity_113_1_000 "lowTemperature -7.6,highTemperature+28.0province Ontario"
#define INVALUEcity_113_1_001 "lowTemperature -7.6,highTemperature +28.0 province Ontario"
#define INVALUEcity_113_1_010 "lowTemperature -7.6, highTemperature +28.0province Ontario"
#define INVALUEcity_113_1_011 "lowTemperature -7.6, highTemperature +28.0 province Ontario"

#define INVALUEcity_113_2_000 "lowTemperature -7.6highTemperature,+28.0province Ontario"
#define INVALUEcity_113_2_001 "lowTemperature -7.6highTemperature, +28.0 province Ontario"
#define INVALUEcity_113_2_010 "lowTemperature -7.6 highTemperature, +28.0province Ontario"
#define INVALUEcity_113_2_011 "lowTemperature -7.6 highTemperature, +28.0 province Ontario"

#define INVALUEcity_113_3_001 "lowTemperature -7.6highTemperature ,+28.0 province Ontario"
#define INVALUEcity_113_3_010 "lowTemperature -7.6 highTemperature ,+28.0province Ontario"
#define INVALUEcity_113_3_011 "lowTemperature -7.6 highTemperature ,+28.0 province Ontario"

#define INVALUEcity_113_4_001 "lowTemperature -7.6highTemperature +28.0, province Ontario"
#define INVALUEcity_113_4_010 "lowTemperature -7.6 highTemperature +28.0,province Ontario"
#define INVALUEcity_113_4_011 "lowTemperature -7.6 highTemperature +28.0, province Ontario"

#define INVALUEcity_113_5_001 		"lowTemperature -7.6highTemperature +28.0 ,province Ontario"
#define INVALUEcity_113_5_011		"lowTemperature -7.6 highTemperature +28.0 ,province Ontario"
#define INVALUEcity_113_55_0115 	"lowTemperature -7.6 highTemperature +28.0  ,,province Ontario     "
#define INVALUEcity_113_2225_011 	"lowTemperature -7.6 highTemperature,,, +28.0 ,province Ontario"
#define INVALUEcity_113_2225_021 	"lowTemperature -7.6 highTemperature,,,  +28.0 ,province Ontario"
#define INVALUEcity_113_22256_021 	"lowTemperature -7.6 highTemperature,,,  +28.0 ,province Ontario,"
#define INVALUEcity_113_12225_021 	"lowTemperature -7.6 highTemperature,,,  +28.0 ,province Ontario"
#define INVALUEcity_113_122256_021 	"lowTemperature -7.6 highTemperature,,,  +28.0 ,province Ontario,"

#define INVALUEcars_113_c1c1		"brand BMW, , price 43210.98"
#define INVALUEcars_113_c1c10c		"brand BMW, , price 43210.98,"
#define INVALUEcars_113_c1c10c1		"brand BMW, , price 43210.98, "

#define INVALUEstudents_113_c1ccc1	"id 991234567, ,,, Grade 87.6"

/*
2: 1 Spelling mistake: All must produce INVALID returns

2.1.1. table name
2.1.2. column name
*/

#define IN2111_TABLE1NAME	"subwayLine"
#define IN2112_TABLE1NAME	"ubwayLines"
#define IN2113_TABLE1NAME	"subwaLines"
#define IN2114_TABLE1NAME	"subwaylines"
#define IN2115_TABLE1NAME	"Subwaylines"
#define IN2116_TABLE1NAME	"subwayLiness"
#define IN2117_TABLE1NAME	"subwayLineS"

#define IN2111_TABLE2NAME	"citie"
#define IN2112_TABLE2NAME	"ities"
#define IN2113_TABLE2NAME	"cites"
#define IN2114_TABLE2NAME	"Cities"
#define IN2115_TABLE2NAME	"CitieS"
#define IN2116_TABLE2NAME	"citieS"
#define IN2117_TABLE2NAME	"ciTies"

#define IN2111_TABLE3NAME	"car"
#define IN2112_TABLE3NAME	"carss"
#define IN2113_TABLE3NAME	"scars"
#define IN2114_TABLE3NAME	"Cars"
#define IN2115_TABLE3NAME	"carS"
#define IN2116_TABLE3NAME	"Car"
#define IN2117_TABLE3NAME	"cArs"

#define IN2111_TABLE4NAME	"student"
#define IN2112_TABLE4NAME	"studentss"
#define IN2113_TABLE4NAME	"sstudents"
#define IN2114_TABLE4NAME	"Students"
#define IN2115_TABLE4NAME	"stuDents"
#define IN2116_TABLE4NAME	"studentS"
#define IN2117_TABLE4NAME	"Student"

/*Column name spelling*/
#define IN2121_T1C1		"nam Bloor Danforth,stops 31,kilometres 26.2"	
#define IN2122_T1C1		"namE Bloor Danforth,stops 31,kilometres 26.2"
#define IN2123_T1C1		"Name Bloor Danforth,stops 31,kilometres 26.2"
#define IN2124_T1C1		"naMe Bloor Danforth,stops 31,kilometres 26.2"
#define IN2125_T1C1		"nams Bloor Danforth,stops 31,kilometres 26.2"
#define IN2126_T1C1		"nams Bloor Danforth,stops 31 ,kilometres 26.2"
#define IN2127_T1C1		"names Bloor Danforth,stops 31 ,kilometres 26.2"
#define IN2128_T1C1		"nname Bloor Danforth,stops 31 ,kilometres 26.2"

#define IN2121_T1C2		"name Bloor Danforth,stop 31,kilometres 26.2"
#define IN2122_T1C2		"name Bloor Danforth,stopS 31,kilometres 26.2"
#define IN2123_T1C2		"name Bloor Danforth,Stops 31,kilometres 26.2"
#define IN2124_T1C2		"name Bloor Danforth,tops 31,kilometres 26.2"
#define IN2125_T1C2		"name Bloor Danforth,stoPs 31,kilometres 26.2"
#define IN2126_T1C2		"name Bloor Danforth,sstops 31,kilometres 26.2"

#define IN2121_T1C3		"name Bloor Danforth,stops 31,kilometre 26.2"
#define IN2122_T1C3		"name Bloor Danforth,stops 31,kilometress 26.2"
#define IN2123_T1C3		"name Bloor Danforth,stops 31,kilometreS 26.2"
#define IN2124_T1C3		"name Bloor Danforth,stops 31,KilometreS 26.2"
#define IN2125_T1C3		"name Bloor Danforth,stops 31,ilometreS 26.2"
#define IN2126_T1C3		"name Bloor Danforth,stops 31, ilometreS 26.2"
#define IN2127_T1C3		"name Bloor Danforth,stops 31, ilometres 26.2"
#define IN2128_T1C3		"name Bloor Danforth,stops 31, kiloMetres 26.2"
#define IN2129_T1C3		"name Bloor Danforth,stops 31, kkilometres 26.2"

#define IN2121_T2C1		"lowtemperature -7.6,highTemperature +28.0,province Ontario"
#define IN2122_T2C1		"LowTemperature -7.6,highTemperature +28.0,province Ontario"
#define IN2123_T2C1		"lowTemperatures -7.6,highTemperature +28.0,province Ontario"
#define IN2124_T2C1		"lowTempErature -7.6,highTemperature +28.0,province Ontario"
#define IN2125_T2C1		"slowTemperature -7.6,highTemperature +28.0,province Ontario"
#define IN2126_T2C1		"lowTemperatur -7.6,highTemperature +28.0,province Ontario"

#define IN2121_T2C2		"lowTemperature -7.6,hightemperature +28.0,province Ontario"
#define IN2122_T2C2		"lowTemperature -7.6,HighTemperature +28.0,province Ontario"
#define IN2123_T2C2		"lowTemperature -7.6,highTemperatur +28.0,province Ontario"
#define IN2124_T2C2		"lowTemperature -7.6,highTemperaturr +28.0,province Ontario"
#define IN2125_T2C2		"lowTemperature -7.6,ighTemperaturr +28.0,province Ontario"
#define IN2126_T2C2		"lowTemperature -7.6,ighTemperature +28.0, province Ontario"
#define IN2127_T2C2		"lowTemperature -7.6,thighTemperature +28.0, province Ontario"
#define IN2128_T2C2		"lowTemperature -7.6,highTempErature +28.0, province Ontario"

#define IN2121_T2C3		"lowTemperature -7.6,highTemperature +28.0,Province Ontario"
#define IN2122_T2C3		"lowTemperature -7.6,highTemperature +28.0, Province Ontario"
#define IN2123_T2C3		"lowTemperature -7.6,highTemperature +28.0,rovince Ontario"
#define IN2124_T2C3		"lowTemperature -7.6,highTemperature +28.0,provinces Ontario"
#define IN2125_T2C3		"lowTemperature -7.6,highTemperature +28.0,provinc Ontario"
#define IN2126_T2C3		"lowTemperature -7.6,highTemperature +28.0,proVince Ontario"
#define IN2127_T2C3		"lowTemperature -7.6,highTemperature +28.0,pprovince Ontario"
#define IN2128_T2C3		"lowTemperature -7.6,highTemperature +28.0,provinced Ontario"

#define IN2121_T3C1		"bran BMW,price 43210.98"
#define IN2122_T3C1		"brans BMW,price 43210.98"
#define IN2123_T3C1		"Brand BMW,price 43210.98"
#define IN2124_T3C1		"brandy BMW,price 43210.98"
#define IN2125_T3C1		"brAnd BMW,price 43210.98"
#define IN2126_T3C1		"bbrand BMW,price 43210.98"
#define IN2127_T3C1		"rand BMW,price 43210.98"

#define IN2121_T3C2		"brand BMW,pric 43210.98"
#define IN2122_T3C2		"brand BMW,prics 43210.98"
#define IN2123_T3C2		"brand BMW,prices 43210.98"
#define IN2124_T3C2		"brand BMW,pricE 43210.98"
#define IN2125_T3C2		"brand BMW,Price 43210.98"
#define IN2126_T3C2		"brand BMW,rice 43210.98"
#define IN2127_T3C2		"brand BMW,priCe 43210.98"
#define IN2128_T3C2		"brand BMW,prie 43210.98"

#define IN2121_T4C1		"i 991234567,Grade 87.6"
#define IN2122_T4C1		"iD 991234567,Grade 87.6"
#define IN2123_T4C1		"ids 991234567,Grade 87.6"
#define IN2124_T4C1		"Id 991234567,Grade 87.6"
#define IN2125_T4C1		"d 991234567,Grade 87.6"
#define IN2126_T4C1		"ID 991234567,Grade 87.6"
#define IN2127_T4C1		"iid 991234567,Grade 87.6"

#define IN2121_T4C2		"id 991234567,grade 87.6"
#define IN2122_T4C2		"id 991234567 ,Grades 87.6"
#define IN2123_T4C2		"id 991234567,rade 87.6"
#define IN2124_T4C2		"id 991234567,Grade 87.6"
#define IN2125_T4C2		"id 991234567,GraDe 87.6"
#define IN2126_T4C2		"id 991234567, GradE 87.6"

/*2.2.2 - 2 invalid columns*/
#define INVALUE2221_T1C1C2	"nam Bloor Danforth,stop 31,kilometres 26.2"	
#define INVALUE2222_T1C1C2	"namE Bloor Danforth,stoops 31,kilometres 26.2"
#define INVALUE2223_T1C1C3	"Name Bloor Danforth,stops 31,kilometre 26.2"
#define INVALUE2224_T1C1C3	"naMe Bloor Danforth,stops 31,dilometres 26.2"

#define INVALUE2225_T1C2C3	"name Bloor Danforth,sops 31,Kilometre 26.2"
#define INVALUE2226_T1C2C3	"name Bloor Danforth,topss 31, kiloMetres 26.2"

#define INVALUE222_T2C1C2	"lowtemperature -7.6,hightemperature +28.0,province Ontario"
#define INVALUE222_T2C1C3	"lowtemperature -7.6,highTemperature +28.0,Province Ontario"
#define INVALUE222_T2C2C3	"lowTemperature -7.6,highTemperatures +28.0,rovinces Ontario"

#define INVALUE222_T3	"bran BMW,prie 43210.98"
#define INVALUE222_T4	"i 991234567,grade 87.6"

/*2.2.3 (redundant): use 2.1.2. def'ns.*/
/*2.3: Use existing constants*/

/* 3.2. Missing column params */
#define INVALUE_32_tb4_t1c1	"stops 31,kilometres 26.2"	
#define INVALUE_32_tb4_t1c1b 	"name,stops 31, kilometres 26.2"
#define INVALUE_32_tb4_t1c2	"name Bloor Danforth,kilometres 26.2"

/*3.3 is redundant.*/

#define INVALUE_411	"name 0.3 ,stops 31,kilometres 26.2"
#define INVALUE_412	"name 99999999999 ,stops 31,kilometres 26.2"
#define INVALUE_413	"name -99999999999 ,stops 31,kilometres 26.2"

#define INVALUE_414	"name Bloor Danforth,stops lalala ,kilometres 26.2"
#define INVALUE_415	"name Bloor Danforth,stops 5.99 ,kilometres 26.2"
#define INVALUE_416	"name Bloor Danforth,stops -99 ,kilometres 26.2"

#define INVALUE_417	"name Bloor Danforth,stops 31,kilometres -26.2"
#define INVALUE_418	"name Bloor Danforth,stops 31,kilometres 26"
#define INVALUE_419	"name Bloor Danforth,stops 31,kilometres -26"

