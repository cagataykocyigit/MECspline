#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <math.h>

int main() {
	// Noktalari dosyadan almak icin "result" adinda dinamik yapili bir dizi olusturuyoruz.
	float* result = malloc(sizeof(float) * 40);
	FILE* dosya;
	// dosyadan aldigimiz noktalari tutan " a " degiskeni.
	int a = 0;
	dosya = fopen("veri.txt", "r");

	if (dosya == NULL) {

		printf("Dosya bulunamadi...");
		return 0;
	}

	else {
		printf("Giris :\n");

		while (!feof(dosya)) {

			fscanf(dosya, "%f", &result[a]);


			//printf("%.0f    ", result[a]);

			a++;


		}


	}
	printf("\n");


	float* x = malloc(sizeof(float) * a);
	float* y = malloc(sizeof(float) * a);
	//Spline icin kullancagimiz diziyi burada yapiyoruz cünkü mec icin kullandigimiz x ve y dizisinin orijinal yapsini bozmak zorunda kaliyoruz.... :(
	float* spline_x = malloc(sizeof(float) * 40);
	float* spline_y = malloc(sizeof(float) * 40);
	int temp1 = 0;
	int temp1_spline = 0;
	int temp2_spline = 0;
	// spline icin kullanacagimiz spline_x ve spline_y dizilerinin atamasi.. :)
	for (int i = 0; i < a; i++) {
		if (i % 2 == 0) {
			spline_x[temp1_spline] = result[i];
			temp1_spline++;
		}


	}

	for (int i = 0; i < a; i++) {
		if (i % 2 == 1) {

			spline_y[temp2_spline] = result[i];
			temp2_spline++;
		}

	}
	/////////////////////////////////////////////////////////////////

	// mec icin kullandigimiz x ve y dizisinin atamalari
	for (int i = 0; i < a; i++) {
		if (i % 2 == 0) {
			x[temp1] = result[i];
			temp1++;
		}


	}
	int temp2 = 0;
	for (int i = 0; i < a; i++) {
		if (i % 2 == 1) {

			y[temp2] = result[i];
			temp2++;
		}

	}
	/////////////////////////////////////////////////////


	for (int i = 0; i < a / 2; i++) {

		//printf("X :%.0f   ", x[i]);
		printf("{ %.0f , %.0f }  ", x[i], y[i]);

	}

	printf("\n");

	/*for (int i = 0;i < a / 2; i++) {
		printf("Y :%.0f  ", y[i]);
	}*/

	fclose(dosya);

	//Dosya islemleri bu noktada bitiyor.

	//Grafik icin kullanacagimiz ekrani acmak icin allegro dan faydalaniyoruz.

	int width = 960;
	int height = 960;
	ALLEGRO_DISPLAY* display = NULL;

	if (!al_init())
		return -1;

	display = al_create_display(width, height);

	if (!display) {
		return -1;
	}




	al_init_primitives_addon();
	al_clear_to_color(al_map_rgb(0, 0, 0));
	// X ve Y eksenlerini olusturan cizgilerimizi cizdiriyoruz.
	al_draw_line(0, 480, 960, 480, al_map_rgb(255, 255, 255), 5);
	al_draw_line(480, 0, 480, 960, al_map_rgb(255, 255, 255), 5);
	//ekrani karelere boluyoruz...
	for (int i = 0; i < 60; i++) {
		if (i != 30) {

			al_draw_line(i * 16, 0, i * 16, 960, al_map_rgb(40, 40, 40), 1);
			al_draw_line(0, i * 16, 960, i * 16, al_map_rgb(40, 40, 40), 1);
		}


	}




	int i = 1;
	int j = 1;
	while (i * 16 < 960)//x ye y doðrularý için kýsa çizgiler
	{
		al_draw_line(i * 16, 474, i * 16, 486, al_map_rgb(255, 255, 255), 3);
		i++;
	}
	while (j * 16 < 960)
	{
		al_draw_line(474, j * 16, 486, j * 16, al_map_rgb(255, 255, 255), 3);
		j++;

	}
	// Dosyadan aldigimiz noktalarý grafik üstünde gosteriyoruz...
	for (int i = 0; i < a; i++) {

		al_draw_circle((x[i] * 16) + 480, 480 - (y[i] * 16), 1, al_map_rgb(229, 20, 0), 10);


	}
	//Dosyadan aldigimiz noktalarin birbirlerine olan uzakliklarina bakiyoruz...
	int k = 0;
	int m = 0;

	float c = 0;
	float d = 0;
	int tutucu_1 = 0;
	int tutucu_2 = 0;
	float max_uzaklik = 0;
	printf("\n");
	// t yi nokta kontrolü yaparken tekrarlamamak icin kullanýyoruz...
	int t = 0;

	for (k = 0; k < (a / 2) - 1; k++) {

		for (m = 1 + t; m <= (a / 2) - 1; m++) {

			c = pow((x[k]) - x[m], 2) + pow(y[k] - y[m], 2);

			d = sqrt(c);



			if (d > max_uzaklik) {

				max_uzaklik = d;
				tutucu_1 = k; //merkez noktasi için index tutuyo
				tutucu_2 = m; //çemberin üstündeki nokta için

			}

		}
		t++;
	}

	//printf("en uzak mesafe : %.2f", max_uzaklik);
	//printf("\n");
	//printf("tutucu 1 : %d   ", tutucu_1);
	//printf("\n");
	//printf("tutucu 2 :%d   ", tutucu_2);
	//printf("\n");

	// Minimum enclosing circle islemlerine burda basliyoruz...
	float yaricap_uzaklik = 0;
	float yaricap = 0;
	float egim = 0;

	//printf("\n");


	egim = (float)(y[tutucu_1] - y[tutucu_2]) / (x[tutucu_1] - x[tutucu_2]);
	//printf("egim :%.1f", egim);
	//printf("\n");


	//printf("\n");
	yaricap_uzaklik = powf((x[tutucu_1]) - x[tutucu_2], 2) + powf(y[tutucu_1] - y[tutucu_2], 2);


	yaricap = sqrt(yaricap_uzaklik);

	//printf("yaricap : %f", yaricap);
	//printf("\n");
	//float nokta_kontrol = 0;
	//float kontrol = 0;
	int z = 0;
	//float ak = 1;
	float yeni_uzaklik = 0;

	// dosya islemlerinden alip result dizisine attigimiz noktalarý x ve y dizilerine ayirmak icin iki farkli dinamik dizi tanimliyoruz "array_x ve array_y".
	float* array_x = malloc(sizeof(float) * 20);
	float* array_y = malloc(sizeof(float) * 20);

	int degisken_1 = 0;

	for (int i = 0; i < (a / 2); i++) {
		if (x[i] != x[tutucu_2]) {

			array_x[degisken_1] = x[i];
			array_y[degisken_1] = y[i];
			degisken_1++;
		}


	}

	/*for (int i = 0; i < (a / 2)-1; i++) {
		printf(" a :%f\n", array_x[i]);
		printf(" b :%f\n", array_y[i]);
	}*/

	// cemberin ustundeki 2. noktayi alabilmek icin index tutan degisken...
	int ikinci_tutucu = 0;


	while (yeni_uzaklik < yaricap_uzaklik) {


		for (int p = 0; p < a / 2; p++) {

			if (yeni_uzaklik < yaricap_uzaklik) {

				x[tutucu_1] = x[tutucu_1] - ((x[tutucu_1] - x[tutucu_2])) / 1000000;
				y[tutucu_1] = y[tutucu_1] - ((y[tutucu_1] - y[tutucu_2])) / 1000000;

			}




			yaricap_uzaklik = powf(x[tutucu_1] - x[tutucu_2], 2) + powf(y[tutucu_1] - y[tutucu_2], 2);


		}

		yeni_uzaklik = powf(x[tutucu_1] - array_x[z], 2) + powf(y[tutucu_1] - array_y[z], 2);

		z++;
		ikinci_tutucu = z;
		if (z == (a / 2) - 1) {

			z = 0;
		}



	}
	//printf(" cemberin ustundeki ikinci nokta : %f %f \n", x[ikinci_tutucu], y[ikinci_tutucu]);
	//printf("cemberin ustundeki birinci nokta : %f %f \n", x[tutucu_2], y[tutucu_2]);
	// cember ustundeki bu iki noktanin orta noktasini bulalim ki merkezi o noktaya dogru yaklastirabilelim...

	float orta_nokta_x = (x[ikinci_tutucu] + x[tutucu_2]) / 2;
	float orta_nokta_y = (y[ikinci_tutucu] + y[tutucu_2]) / 2;

	//printf("orta nokta x ve y : %f %f \n", orta_nokta_x, orta_nokta_y);
	//merkezin yaklasmasi gereken noktayi bulduk...

	//printf("cemberin su an sahip oldugu merkez : %f %f \n", x[tutucu_1], y[tutucu_1]);
	//orta noktaya yaklasicak olan merkez...

	// orta noktaya yakalasacagi cizgiyi bulmak icin orta nokta ve merkez arasindaki egimden faydalanicaz...



	 // bu sekilde while ýn icinde kullanicaz ve merkezi orta noktaya dogru yaklsatiricaz...
	// peki nereye kadar yaklastiricaz ?
	// su an elimizde bir yaricap uzakligi var. elimizdeki merkezin cemberin ustundeki iki noktadan herhangi birine olan uzakligi yaricap...

	// yaricap uzakliginini noktalara olan kontrolunu gerceklestirebilmek adina yeni_uzaklik_2 adinda yeni bir degiskken tanimliyoruz...
	float yeni_uzaklik_2 = 0;

	//merkezin noktalar arasindaki uzakliga surekli olarak bakabilmesi icin bir sayac tanimlamaliyiz...
	int tu = 0;
	int kai = 0;



	while (yeni_uzaklik_2 < yaricap_uzaklik) {

		for (int p = 0; p < a / 2; p++) {

			if (yeni_uzaklik_2 < yaricap_uzaklik) {

				x[tutucu_1] = x[tutucu_1] - ((x[tutucu_1] - orta_nokta_x) / 1000000);
				y[tutucu_1] = y[tutucu_1] - ((y[tutucu_1] - orta_nokta_y) / 1000000);




			}
			yaricap_uzaklik = powf(x[tutucu_1] - x[tutucu_2], 2) + powf(y[tutucu_1] - y[tutucu_2], 2);

		}
		yeni_uzaklik_2 = powf(x[tutucu_1] - x[tu], 2) + powf(y[tutucu_1] - y[tu], 2);


		tu++;

		if (tu == (a / 2)) {

			tu = 0;
		}
	}


	yaricap = sqrt(yaricap_uzaklik);
	// CEMBER
	al_draw_circle(480 + (x[tutucu_1] * 16), 480 - (y[tutucu_1] * 16), yaricap * 16, al_map_rgb(139, 34, 82), 3);
	//MERKEZ NOKTASI
	al_draw_circle(480 + (x[tutucu_1] * 16), 480 - (y[tutucu_1] * 16), 1, al_map_rgb(255, 4, 240), 15);
	//YARICAP CIZGISI
	al_draw_line(480 + (x[tutucu_1] * 16), 480 - (y[tutucu_1] * 16), 480 + (x[tutucu_1] * 16) + +yaricap * 16, 480 - (y[tutucu_1] * 16), al_map_rgb(180, 34, 82), 5);

	printf("Cikti :");
	printf("\n");
	printf("\n");

	printf("Merkez = { %.3f , %.3f }", x[tutucu_1], y[tutucu_1]);
	printf("\n");

	printf("Yaricap = %.3f", yaricap);
	printf("\n");
	//printf(" orta nokta x ve y %f %f \n", orta_nokta_x, orta_nokta_y);
	// MEC cizdirme islemimiz burda son buluyor.


	// SPLINE cizdirmeye burda basliyoruz.


	float v = 0;
	float x_drawer = 0;
	float y_drawer = 0;
	int cntr = 0;

	float p0x;
	float p0y;
	float p1x;
	float p1y;
	float midx;
	float midy;
	float midx2;
	float midy2;
	i = 1;
	midx = (spline_x[i] + spline_x[i + 1]) / 2; //Belirtilen noktalarin orta noktalari bulunuyor.
	midy = (spline_y[i] + spline_y[i + 1]) / 2;


	if ((a / 2) == 3) { //sadece 3 nokta girildiginde spline cizdiren dongu ve onun kosulu.
		while (v < 1) {
			/*burada spline cizmek icin ozel bir formul kullaniliyor. 'v' degiskeni basitce spline egrisinin cizim hassasiyetini gosterir.
			ayni formul kodun devaminda da ayni sekilde kullaniliyor.*/
			x_drawer = (1 - v) * (1 - v) * spline_x[0] + 2 * (1 - v) * v * spline_x[1] + v * v * spline_x[2];
			y_drawer = (1 - v) * (1 - v) * spline_y[0] + 2 * (1 - v) * v * spline_y[1] + v * v * spline_y[2];
			al_draw_circle(x_drawer * 16 + 480, 480 - y_drawer * 16, 1, al_map_rgb(25, 25, 112), 4);
			v = v + 0.01;
		}
	}
	else if ((a / 2) == 4) {//sadece 4 nokta girildiginde spline cizdiren dongu ve onun kosulu.
		while (v < 1) {

			x_drawer = ((1 - v) * (1 - v) * (1 - v) * spline_x[0]) + 3 * (1 - v) * (1 - v) * v * spline_x[1] + 3 * (1 - v) * v * v * spline_x[2] + v * v * v * spline_x[3];
			y_drawer = ((1 - v) * (1 - v) * (1 - v) * spline_y[0]) + 3 * (1 - v) * (1 - v) * v * spline_y[1] + 3 * (1 - v) * v * v * spline_y[2] + v * v * v * spline_y[3];
			al_draw_circle(x_drawer * 16 + 480, 480 - y_drawer * 16, 1, al_map_rgb(25, 25, 112), 4);
			v = v + 0.001;
		}
	}


	else {//4ten fazla nokta girildiginde spline cizdiren dongu ve onun kosulu.
		while (v < 1) {

			x_drawer = (1 - v) * (1 - v) * spline_x[0] + 2 * (1 - v) * v * spline_x[1] + v * v * midx;
			y_drawer = (1 - v) * (1 - v) * spline_y[0] + 2 * (1 - v) * v * spline_y[1] + v * v * midy;
			al_draw_circle(x_drawer * 16 + 480, 480 - y_drawer * 16, 1, al_map_rgb(25, 25, 112), 4);
			v = v + 0.001;
		}
		v = 0;
		for (i = 2; i < (a / 2) - 2; i++) {

			midx2 = (spline_x[i] + spline_x[i + 1]) / 2;
			midy2 = (spline_y[i] + spline_y[i + 1]) / 2;

			midx = (spline_x[i - 1] + spline_x[i]) / 2;
			midy = (spline_y[i - 1] + spline_y[i]) / 2;


			while (v < 1) {

				x_drawer = (1 - v) * (1 - v) * midx + 2 * (1 - v) * v * spline_x[i] + v * v * midx2;
				y_drawer = (1 - v) * (1 - v) * midy + 2 * (1 - v) * v * spline_y[i] + v * v * midy2;
				al_draw_circle(x_drawer * 16 + 480, 480 - y_drawer * 16, 1, al_map_rgb(25, 25, 112), 4);
				v = v + 0.001;
			}
			v = 0;


		}


		p0x = spline_x[(a / 2) - 2];//dizinin sondan ikinci elemani alinir.
		p0y = spline_y[(a / 2) - 2];
		p1x = spline_x[(a / 2) - 1];//dizinin son elemani alinir.
		p1y = spline_y[(a / 2) - 1];

		while (v < 1) {
			x_drawer = (1 - v) * (1 - v) * midx2 + 2 * (1 - v) * v * p0x + v * v * p1x;
			y_drawer = (1 - v) * (1 - v) * midy2 + 2 * (1 - v) * v * p0y + v * v * p1y;


			al_draw_circle(x_drawer * 16 + 480, 480 - y_drawer * 16, 1, al_map_rgb(25, 25, 112), 4);
			v = v + 0.001;
		}
	}

	al_flip_display();

	al_rest(400.0);


	al_destroy_display(display);
	return 0;
}