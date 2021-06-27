#include "custom_median_implementation.h"

using namespace std;

Subtractors::imgMedian::imgMedian() {}

Subtractors::imgMedian::imgMedian(unsigned char R_temp) { R = R_temp; }

void Subtractors::imgMedian::InitMedian(cv::Mat &img)
{
	width = static_cast<unsigned int>(img.cols);
	height = static_cast<unsigned int>(img.rows);

	unsigned int len = static_cast<unsigned int>(width * height);
	iterator = 0;
	R_half = static_cast<unsigned char>(R / 2);
	q1 = static_cast<unsigned char>(R / 4);
	q2 = static_cast<unsigned char>(R / 2);
	q3 = static_cast<unsigned char>(q2 + q1);
	vector<unsigned char> clean(R, 0);

	for (unsigned int i = 0; i < len; i++)
	{
		V.push_back(clean);
		NEXT.push_back(clean);
		PREV.push_back(clean);
	}

	localCopy = img.clone();
	for (unsigned int y = 0; y < height; y++) {
		for (unsigned int x = 0; x < width; x++) {
			unsigned int i = x + y * width;

			for (unsigned int c = 0; c < R; c++) {
				V[i][c] = static_cast<unsigned char>(img.at<unsigned char>(
						cv::Point(static_cast<int>(x), static_cast<int>(y))));
				NEXT[i][c] = static_cast<unsigned char>(c + 1);
				PREV[i][c] = static_cast<unsigned char>(c - 1);
			}

			V[i][0] = (img.at<unsigned char>(
					cv::Point(static_cast<int>(x), static_cast<int>(y))));

			NEXT[i][R - 1] = 1;
			PREV[i][1] = (R)-1;

			NEXT[i][0] = 1;
			PREV[i][0] = R - 1;

			// q1:
			// NEXT[i][R] =  img.at<unsigned char>(cv::Point(x,y)) ;
			// q3:
			// PREV[i][R] = img.at<unsigned char>(cv::Point(x,y)) ;
		}
	}
}

void Subtractors::imgMedian::ShowIterator() {
	int i = 0;
	for (std::vector<unsigned char>::iterator it = V[0].begin(); it != V[0].end();
			 it++) {
		std::cout << static_cast<int>(i) << '\t';
		i++;
	}
	std::cout << "\n";
}

void Subtractors::imgMedian::ShowMedianNode(unsigned int temp) {
	// printf("ShowMedianNode()\n");
	for (std::vector<unsigned char>::iterator it = V[temp].begin();
			 it != V[temp].end(); it++) {
		std::cout << static_cast<int>(*it) << '\t';
	}
	std::cout << "\n";
	for (std::vector<unsigned char>::iterator it = NEXT[temp].begin();
			 it != NEXT[temp].end(); it++) {
		std::cout << static_cast<int>(*it) << '\t';
	}
	std::cout << "\n";
	for (std::vector<unsigned char>::iterator it = PREV[temp].begin();
			 it != PREV[temp].end(); it++) {
		std::cout << static_cast<int>(*it) << '\t';
	}
	std::cout << "\n";
}

void Subtractors::imgMedian::RemoveNode() {
	iterator++;
	if (iterator >= R)
		iterator = 1;

	unsigned int xy;
	//#pragma omp parallel for num_threads(2)  schedule(dynamic)

	for (xy = 0; xy < height * width; xy++) {
		unsigned char start = NEXT[xy][0];
		unsigned char stop = PREV[xy][0];

		// int val = img.at<char>(Point(x,y));//15
		if (start == iterator) // usuwamy start
		{
			// przesuwamy start o jeden do przodu:
			unsigned char nextStart = NEXT[xy][start];

			// sprawdzamy jaki był przed startem:
			unsigned char prevStart = PREV[xy][start];

			// zmieniamy poprzedniego aby wskazywał na nowy start:
			NEXT[xy][prevStart] = nextStart;

			// zmieniamy następnego aby wzkazywał start jako poprzedni:
			PREV[xy][nextStart] = prevStart;

			// zmieniamy start:
			NEXT[xy][0] = nextStart;

			// zerujemy wartosci w nastepnym i poprzednim usunietego pola:
			// NEXT[xy][start] = 0;
			// PREV[xy][start] = 0;

			// zmieniamy stop na poprzedni nowego startu:
			PREV[xy][0] = PREV[xy][nextStart];
		} else if (stop == iterator) // usuwamy stop:
		{
			// sprawszamy jaki był przed stopem:
			unsigned char nextStop = PREV[xy][stop];

			// sprawdzamy jaki jest start: Bo po stopie jest start:
			unsigned char prevStop = NEXT[xy][stop];

			// zmieniamy aby nowy stop wskazywał na start:
			NEXT[xy][nextStop] = prevStop;
			// zmieniamy aby start wskazywał na nowy stop:
			PREV[xy][prevStop] = nextStop;

			// zmieniamy stop:
			PREV[xy][0] = nextStop;

			// zerujemy wartości poprzedniego stopu:
			// NEXT[xy][stop] = 0;
			// PREV[xy][stop] = 0;

			// zmieniamy start na następny nowego stopu:
			NEXT[xy][0] = NEXT[xy][nextStop];
		} else {
			// wyznaczamy indexy next i prev:
			unsigned char next = NEXT[xy][iterator];
			unsigned char prev = PREV[xy][iterator];
			// następny poprzedniego wskazuje na następnego:
			NEXT[xy][prev] = next;
			// poprezedni następnego wskazuje na poprzedniego:
			PREV[xy][next] = prev;
			// zerujemy wartości:
			// NEXT[xy][ iterator ] = 0;
			// PREV[i][ iterator ] = 0;
		}
	}
}

void Subtractors::imgMedian::AddImage(cv::Mat &img) {
	// RemoveNode();
	unsigned int x, y;
	//#pragma omp parallel for private (x) num_threads(2) schedule(dynamic)
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			unsigned int i = x + y * width;
			unsigned char start = NEXT[i][0];
			unsigned char stop = PREV[i][0];
			unsigned char val = img.at<unsigned char>(
					cv::Point(static_cast<int>(x), static_cast<int>(y)));
			unsigned int insertRowValue = 0;
			unsigned char nextRow = 0;
			unsigned char median = V[i][0];

			if (val > median) // jezeli val jest wiekszy od mediany: szukaj od góry
			{
				if (val >= V[i][stop]) {
					// nowy stop to aktualna pozycja:
					PREV[i][0] = iterator;

					// następna pozycja wskazuje na start:
					NEXT[i][iterator] = start;
					// poprzednia pozycja wskazuje na starego stopa:
					PREV[i][iterator] = stop;
					// nastepna starego stopa wskazuje na aktualna pozycję:
					NEXT[i][stop] = iterator;
					// poprzednia starego startu wstazuje na aktualną pozycję:
					PREV[i][start] = iterator;
					// wpisz wartość aktualnej pozycji:
					V[i][iterator] = val;
				} else {
					unsigned char insertRow = 0;
					nextRow = stop; // 1

					for (unsigned int c = (R); c > 0; c--) {
						insertRow = PREV[i][nextRow];
						insertRowValue = V[i][nextRow];

						if (val < insertRowValue) {
							nextRow = insertRow;
							break;
						}
						nextRow = insertRow;
					}

					insertRow = nextRow;

					unsigned char nextinsertROW = NEXT[i][insertRow];

					NEXT[i][iterator] = nextinsertROW;
					PREV[i][iterator] = insertRow;

					NEXT[i][insertRow] = iterator;
					PREV[i][nextinsertROW] = iterator;

					V[i][iterator] = val;
				}
			} else if (val <=
								 median) // jezeli val jest mniejszy od mediany: szukaj od dołu
			{
				if (val <= V[i][start]) {
					NEXT[i][0] = iterator;

					NEXT[i][iterator] = start;

					PREV[i][iterator] = stop;

					NEXT[i][stop] = iterator;
					PREV[i][start] = iterator;

					V[i][iterator] = val;
				} else {
					unsigned char insertRow = 0;
					nextRow = start;

					for (int c = 0; c < (R); c++) {
						insertRow = NEXT[i][nextRow];
						insertRowValue = V[i][nextRow];

						if (val <= insertRowValue) {
							break;
						}
						nextRow = insertRow;
					}

					insertRow = nextRow;
					unsigned char previnsertRow = PREV[i][insertRow];

					NEXT[i][iterator] = insertRow;
					PREV[i][iterator] = previnsertRow;

					PREV[i][insertRow] = iterator;
					NEXT[i][previnsertRow] = iterator;

					V[i][iterator] = val;
				}
			}
		}
	}
}

void Subtractors::imgMedian::getMedian(cv::Mat &img) {
	// cv::Mat copy = img.clone();
	unsigned int x, y;
	//#pragma omp parallel for private(x) num_threads(2) schedule(dynamic)
	// shared(V,NEXT,PREV,img)
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			unsigned int i = x + y * width;
			unsigned int start = NEXT[i][0];
			unsigned int nextRow = start;
			unsigned int insertRow = 0;
			for (unsigned int c = 0; c <= q2; c++) {
				insertRow = NEXT[i][nextRow];
				nextRow = insertRow;
			}
			img.at<unsigned char>(cv::Point(static_cast<int>(x),
																			static_cast<int>(y))) = V[i][insertRow];
			V[i][0] = V[i][insertRow];
		}
	}
}

cv::Mat Subtractors::imgMedian::getMedian() {
	// cv::Mat img = localCopy.clone();
	unsigned int x, y;
	//#pragma omp parallel for private(x) num_threads(2) schedule(dynamic)
	// shared(V,NEXT,PREV,img)
	for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			unsigned int i = x + y * width;
			unsigned char start = NEXT[i][0];
			unsigned char nextRow = start;
			unsigned char insertRow = 0;
			for (unsigned int c = 0; c <= q2; c++) {
				insertRow = NEXT[i][nextRow];
				nextRow = insertRow;
			}
			localCopy.at<unsigned char>(cv::Point(
					static_cast<int>(x), static_cast<int>(y))) = V[i][insertRow];
			V[i][0] = V[i][insertRow];
		}
	}
	return localCopy;
}

void Subtractors::imgMedian::UnitTestInit(cv::Mat M, int R,
																					vector<unsigned char> V1,
																					vector<unsigned char> N1,
																					vector<unsigned char> P1,
																					unsigned int interator) {
	InitMedian(M);
	ShowIterator();
	ShowMedianNode(interator);
}

void Subtractors::imgMedian::UnitTestCheck(vector<unsigned char> V1,
																					 vector<unsigned char> N1,
																					 vector<unsigned char> P1) {
	unsigned int i = 0;
	for (std::vector<unsigned char>::iterator it = V[0].begin(); it != V[0].end();
			 ++it) {
		if (V[0][i] != V1[i]) {
			printf("VAL:błąd na iteracji[%d], w V powinno być:%d\n zamiast:%d\n", i,
						 V1[i], V[0][i]);
		}
		i++;
	}
	std::cout << '\n';

	i = 0;
	for (std::vector<unsigned char>::iterator it = NEXT[0].begin();
			 it != NEXT[0].end(); ++it) {
		if (NEXT[0][i] != N1[i]) {
			printf("NEXT:błąd na iteracji[%d], w V powinno być:%d\n zamiast:%d\n", i,
						 N1[i], NEXT[0][i]);
		}
		i++;
	}
	std::cout << '\n';

	i = 0;
	for (std::vector<unsigned char>::iterator it = PREV[0].begin();
			 it != PREV[0].end(); ++it) {
		if (PREV[0][i] != P1[i]) {
			printf("PREV:błąd na iteracji[%d], w V powinno być:%d\n zamiast:%d\n", i,
						 P1[i], PREV[0][i]);
		}
		i++;
	}
	std::cout << '\n';
}

Subtractors::imgMedian::~imgMedian() {}
