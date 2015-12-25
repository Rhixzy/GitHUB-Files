/*
ÇAÐATAY DURMUÞOÐLU
NO:15011602
Proje konusu:Resimlerde Renk Kuantalama

Açýklama:
BMP formatýndaki bir resim dosyasýndaki farklý renk sayýsýnýn kullanýcýnýn
istediði sayýya indirgenmesi yapýlacaktýr. Kümeleme için bottom-up hiyerarþik kümeleme
kullanýlacaktýr. Kullanýcý programa resmin ismini(.bmp uzantisi ile birlikte) ve indirgenmesini istediði
farklý renk sayýsýný girecektir, program resmi iþledikten sonra ayný yere sonuna kullanýcýnýn
girdiði farklý renk sayýsýný ekleyerek kullanýcýnýn girdiði isimde kaydedecektir.


resmi dosyadan okurken kullandigim bilgiler: https://ugurkoltuk.wordpress.com/2010/03/04/an-extreme-simple-pgm-io-api/
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>										//isspace()
#include <math.h>										//fabs()
#define HI(num) (((num) & 0x0000FF00) >> 8)				//resim okumak icin gerekli defineler
#define LO(num) ((num) & 0x000000FF)					//resim okumak icin gerekli defineler
			
typedef struct _PGMData {
	int row;
	int col;
	int max_gray;
	int **matrix;
} PGMData;

int **allocate_dynamic_matrix(int, int);
void SkipComments(FILE*);
PGMData* readPGM(const char*,PGMData*);
void writePGM(const char*,const PGMData*);

void bubbleSort(float *distanceArray,int boyut);
float** distanceMatrice(int boyut, float distanceArray[]);
void printArray(float **, int);
void updateUnique(float *sortedArray, float ort, float sayi1, float sayi2, int boyut);
void updateMatrice(float **matrice, float ort, float sayi1, float sayi2);
float minValue(float **dist, int *satir, int *sutun, int boyut);


int n,m;												//resimde bulunan yukseklik ve genislik degerlerini tutucak olan degiskenlerim n=row   m=column			

int main(){
	PGMData *kappa;										//pgm dosyasinin bilgilerini tutucak olan pgmdata
	kappa = (PGMData*)malloc(sizeof(PGMData));
	char pgmINPUT[30],pgmOUTPUT[30];					//girdi ve cikti olacak olan resimlerin isimleri
	printf("Resmin ismini uzantisi ile birlikte giriniz :");
	gets(pgmINPUT);
	kappa = readPGM(pgmINPUT, kappa);
	printf("Olusturalacak olan resmin ismini uzantisi ile birlikte giriniz :");
	gets(pgmOUTPUT);
	int elemanSayisi;
	float **matrice;									//renk degerlerini tutucak olan matris								
	float *sortedArray;									//unique degerlerimizi (sadece 1 tane olan, aynisindan baska elemani bulunmayan) degiskenleri tut
	float *distanceArray;								//butun uzakliklarin degerlerini tutucak olan array
	int renkSayisi;
	int i = 0, j = 0, k = 0, temp = 0;
	n=kappa->row;										//n'e editlenilecek resmin satir sayisini ata
	m=kappa->col;										//n'e editlenilecek resmin sutun sayisini ata
	
	matrice= (float **)malloc(n*sizeof(float *)); 		//matrise hafizadan olarak yer ayýr
	for (i = 0; i < m; i++)
	{
		matrice[i] =(float*)malloc(sizeof(float)*m);	
	}
	
	if(matrice==NULL)									//kontrol
	{
		printf("Matrise Bellekte yer acilamadi!\n");
	}
	
	for(i=0;i<n;i++){
		for(j=0;j<m;j++){
			matrice[i][j]=kappa->matrix[i][j];			//PGMData struct'i icinde tuttugumuz matrix'ten elemanlari teker teker uzerinde islemler yapicagimiz "matrice" dizisine yolla
		}
	}
	
	distanceArray=(float*)malloc(sizeof(float)*(m*n));  //distanceArray icin hafýzadan yer ac
	
	if(distanceArray==NULL)								//kontrol
	{
		printf("distanceArray Bellekte yer acilamadi!\n");
	}
	
	sortedArray=(float*)malloc(sizeof(float)*(m*n));  	//sortedArray icin bellekten yer ac
	if(sortedArray==NULL)
	{
		printf("Sirali Bellekte yer acilamadi!\n");		//kontrol
	}
	
	printf("Kac adet renge dusurmek istedigizi girin : ");		//algoritma sonucu kac farkli renk kumesinin kalcagina belirleyen degiskeni kullanicidan gelecek degeri al
	scanf("%d",&renkSayisi);
	
	for (i = 0; i<n; i++)    							//distanceArray matrice'nin icerigini alarak baslangic yap
	{
		for (j = 0; j<m; j++)
		{
			distanceArray[k] = matrice[i][j];
			k++;
		}
	}
	
    bubbleSort(distanceArray,m*n);
	k = 0;       										//sortedArray'yi unique olan bir array yapmak icin k'yi 0 olarak atayip islemlere devam et
	i = 0;
	sortedArray[k] = distanceArray[i++];
	
	while (i<(m*n))										
	{
		if (distanceArray[i] == sortedArray[k])			//sirali bir sekilde sortedArray'yin elemanlarini distanceArray'den al
		{
			i++;
			continue;
		}
		else{
			sortedArray[++k] = distanceArray[i];
			i++;

		}
	}
	
    elemanSayisi = k+1; 								//sortedArray unique dizisinin boyu
	float **dist=NULL;									//mesafelerin hepsini tutucak olan matris
	float min,ort;
	int bas = 0, bitis = 0;
	int *satir = &bas, *sutun = &bitis;
	int l=0;
	int size=elemanSayisi+1;							
	while (size-1>renkSayisi){							//ana dongu
        
		dist = distanceMatrice(size, sortedArray);  								//mesafelerin hepsini tutucak olan matrisi olustur
		min = minValue(dist, satir, sutun, size);									//birbirine en yakin olaniiki index'i bul
		ort = (dist[*satir][0] + dist[0][*sutun]) / 2;								//birbirlerine yakin olan indexlerin ortalamalarini al
		updateUnique(sortedArray, ort, dist[*satir][0], dist[0][*sutun], size);		//sortedArray'i degisecek olan degerler icin guncelle
		size--;
		updateMatrice(matrice, ort, dist[*satir][0], dist[0][*sutun]);				//matrice'yi(renk degerlerini tutan matris) guncelle
	}
	for(i=0;i<n;i++){
		for(j=0;j<m;j++){
			kappa->matrix[i][j]=matrice[i][j];									//PGMData structu'nin icindeki matrix'e matrice'de tutulan(bottom-up hiyerarþi uylulanmis bulunan degerler) degerleri kopyala 
		}
	}
	writePGM(pgmOUTPUT, kappa);													//PGMData structu uzerinde tutulan degerleri kullanarak yeni remi olustur
	
	for(i=0;i<n;i++)															//dinamik acilan hafizalari temizle
		free(matrice[i]);
	free(matrice);
	free(distanceArray);
	free(sortedArray);
	
	return 0;
}

int **allocate_dynamic_matrix(int row, int col)									//girilen resim icin dinamik olaran hafizada yer acan fonksiyon
{
	int **ret_val;
	int i;

	ret_val = (int **)malloc(sizeof(int *) * row);
	if (ret_val == NULL) {
		perror("memory allocation failure");
		exit(EXIT_FAILURE);
	}

	for (i = 0; i < row; ++i) {
		ret_val[i] = (int *)malloc(sizeof(int) * col);
		if (ret_val[i] == NULL) {
			perror("memory allocation failure");
			exit(EXIT_FAILURE);
		}
	}

	return ret_val;
}


float** distanceMatrice(int size, float distanceArray[])	//mesafele degerlerini tutucak olan float** donduren fonksiyon
{
	int i, j, k = 0;
	float temp;
	float **dist; 										

	dist = (float **)malloc((size)*sizeof(float *));	//hafizadan sortedArray'deki farkli eleman sayisi kadar matris olustur

	for (i = 0; i < (size); i++)
	{
		dist[i] = (float *)malloc(sizeof(float)*(size));
	}



	for (i = 1; i < size; i++) 							//dist matrisinin icini dsimetrik bir sekilde 
	{
		dist[i][0] = distanceArray[k];
		dist[0][i] = distanceArray[k];
		k++;
	}


	for (i = 1; i < size; i++)
	{
		for (j = 1; j < size; j++)
		{
			
			if (i<j)
			{
                temp = dist[i][0] - dist[0][j];			//dist matrisinin icine mesafeleri duzgunce yerlestir
				dist[i][j] = fabs(temp);
			}
		}

	}

	return dist;

}

/*void printArray(float **matrice, int boyut)					//kontrol amacli onceden yazilan print
{
	int i, j;
	printf("\n\n");
	for (i = 0; i < boyut; i++)
	{
		for (j = 0; j < boyut; j++)
		{
			if (i<j)
				printf("%5.2f", matrice[i][j]);

			else if (j == 0 && i != 0)
				printf("%5.2f", matrice[i][j]);

			else
				printf("     ");
		}
		printf("\n");
	}

}*/

float minValue(float **dist, int *satir, int *sutun, int boyut)	//dist matrisi icindeki birbirlerine mesafe bakimindan en yakin bulunan iki elemanin
{																//index'lerini bulup saklayan ve bu degeri bulup cagirana geri donduren fonksiyon
	float min = n*m;
	int i, j;

	for (i = 1; i < boyut; i++)
	{
		for (j = i+1; j < boyut; j++)
		{
			if (i<j && min>dist[i][j])
			{
				min = dist[i][j];
				*satir = i;
				*sutun = j;
			}

		}
	}

	return min;
}

void deallocate_dynamic_matrix(int **matrix, int row)						//girilen resim icin dinamik acilan hafizayi free'leyen fonksiyon
{
	int i;

	for (i = 0; i < row; ++i)
		free(matrix[i]);
	free(matrix);
}

void SkipComments(FILE *fp)													//PGM'yi okurken comment degerlerini atlayip bizim icin gereken row,column... tarzi degiskenleri PGM'den almamiza yardim eden fonksiyon
{
	int ch;
	char line[100];

	while ((ch = fgetc(fp)) != EOF && isspace(ch))
		;
	if (ch == '#') {
		fgets(line, sizeof(line), fp);
		SkipComments(fp);
	}
	else
		fseek(fp, -1, SEEK_CUR);
}

PGMData* readPGM(const char *file_name, PGMData *data)					   //girilecek olan resim dogru formatta ise gereken row,column,max_gray,**matrix degiskenlerini dolduran fonksiyon			
{
	FILE *pgmFile;
	char version[3];
	int i, j;
	int lo, hi;

	pgmFile = fopen(file_name, "rb");
	if (pgmFile == NULL) {
		perror("cannot open file to read");
		exit(EXIT_FAILURE);
	}

	fgets(version, sizeof(version), pgmFile);
	if (strcmp(version, "P5")) {
		fprintf(stderr, "Wrong file type!\n");
		exit(EXIT_FAILURE);
	}

	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->col);
	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->row);
	SkipComments(pgmFile);
	fscanf(pgmFile, "%d", &data->max_gray);
	fgetc(pgmFile);

	data->matrix = allocate_dynamic_matrix(data->row, data->col);
	if (data->max_gray > 255)
		for (i = 0; i < data->row; ++i)
			for (j = 0; j < data->col; ++j) {
				hi = fgetc(pgmFile);
				lo = fgetc(pgmFile);
				data->matrix[i][j] = (hi << 8) + lo;
			}
	else
		for (i = 0; i < data->row; ++i)
			for (j = 0; j < data->col; ++j) {
				lo = fgetc(pgmFile);
				data->matrix[i][j] = lo;
			}

	fclose(pgmFile);
	return data;
}

void writePGM(const char *filename, const PGMData *data)					//PGMData *data'nin icini doldurup pgm olusturan fonksiyon
{
	FILE *pgmFile;
	int i, j;
	int hi, lo;

	pgmFile = fopen(filename, "wb");
	if (pgmFile == NULL) {
		perror("cannot open file to write");
		exit(EXIT_FAILURE);
	}

	fprintf(pgmFile, "P5 ");
	fprintf(pgmFile, "%d %d ", data->col, data->row);
	fprintf(pgmFile, "%d ", data->max_gray);

	if (data->max_gray > 255) {
		for (i = 0; i < data->row; ++i) {
			for (j = 0; j < data->col; ++j) {
				hi = HI(data->matrix[i][j]);
				lo = LO(data->matrix[i][j]);
				fputc(hi, pgmFile);
				fputc(lo, pgmFile);
			}

		}
	}
	else {
		for (i = 0; i < data->row; ++i)
			for (j = 0; j < data->col; ++j) {
				lo = LO(data->matrix[i][j]);
				fputc(lo, pgmFile);
			}
	}

	fclose(pgmFile);
	deallocate_dynamic_matrix(data->matrix, data->row);
}


void updateUnique(float *sortedArray, float ort, float sayi1, float sayi2, int boyut){		//sortedArray'de, onceden bulunan ve birbirlerine en yakin olan iki farkli index'lerde bulunan 	
	int i,index=0;																			//degerlerin ortalamarini alip bu iki farkli indexte bulunan sayilari bu ortalamaya esitleyen ve	
	for (i = 0; i<boyut-1; i++)																//esitledikten sonra bunlari tekrar duzgun bir siraya sokan ve tekrardan unique olmasini saglayan fonksiyon
	{																						

		if (sortedArray[i] == sayi1 || sortedArray[i] == sayi2)
		{
			sortedArray[i] = ort;
		}
		
	}
	
	bubbleSort(sortedArray,boyut);
		
	for (i = 0; i < boyut-1 ; i++){	
				//sortedArray diziyi guncelle
		if (sortedArray[i] == sortedArray[i + 1]){											//iki tane index ort degerine esit oldugu icin sortedArray artik unique bir dizi olmaktan cikti
			index = i;																		//bunu duzeltmek icin yandaki algoritmayi yazdim
			break;
		}
	}
	
	for (i = index; i < boyut-1 ; i++){
		sortedArray[i] = sortedArray[i + 1];
	}	
}

void updateMatrice(float **matrice, float ort, float sayi1, float sayi2){					//degisen renk degerlerini(birbirlerine mesafe bakimindan en yakin olan renk degerlerinin ortalamalari alindigi icin renk degerleri degisti) 
	int i, 																					//**matrice matrisimizin ustune uzerinde editleyen fonksiyon

		j;

	for (i = 0; i<n; i++)
	{
	for (j = 0; j<m; j++)
		{

			if (matrice[i][j] == sayi1 || matrice[i][j] == sayi2)
			{
				matrice[i][j] = ort;
			}
		}
	}

}

void bubbleSort(float *distanceArray,int boyut)								//fonksiyona gelen diziyi sort eden bubblesort
{
    int i,j;
    float temp;
    
    for (i = 1; i<boyut; i++) 
	{
		for (j = 0; j<(boyut-1) ; j++)
		{
			if (distanceArray[j]>distanceArray[j + 1])
			{
				temp = distanceArray[j];
				distanceArray[j] = distanceArray[j + 1];
				distanceArray[j + 1] = temp;


			}

		}
	}     
}
