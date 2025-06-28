// Пополнение и использование библиотеки фигур
//связь с ОС (пример для Visual C++2017)
#include <locale.h>
#include <iostream>
#include "screen.h"
#include "shape.h"
// ПРИМЕР ДОБАВКИ: дополнительный фрагмент – полуокружность
class h_circle: public rectangle, public reflectable {
public:
	h_circle(point a, int rd) 
		: rectangle(point(a.x-rd, a.y), point(a.x+rd,a.y+rd*0.7+1)) { }
	void draw();
	void flip_horisontally() { };   // Отразить горизонтально (пустая функция)
	void rotate_right() {}     // Повернуть вправо 
	void rotate_left() {}      // Повернуть влево
};

void h_circle :: draw()   //Алгоритм Брезенхэма для окружностей
{  // (выдаются два сектора, указываемые значением reflected::vert)
    int x0 = (sw.x+ne.x)/2, y0 = vert ? ne.y : sw.y;
	int radius = (ne.x - sw.x)/2;
	int x = 0, y = radius, delta = 2 - 2 * radius, error = 0;
    while(y >= 0) { // Цикл рисования
	    if(vert) { put_point(x0 + x, y0 - y*0.7); put_point(x0 - x, y0 - y*0.7); }
		else { put_point(x0 + x, y0 + y*0.7); put_point(x0 - x, y0 + y*0.7); }
        error = 2 * (delta + y) - 1;
        if(delta < 0 && error <= 0) { ++x; delta += 2 * x + 1; continue; }
        error = 2 * (delta - x) - 1;
        if(delta > 0 && error > 0) { --y; delta += 1 - 2 * y; continue; }
        ++x; delta += 2 * (x - y);  --y;
	}
//	rectangle::draw();
}
// ПРИМЕР ДОБАВКИ: дополнительная функция присоединения…
void down(shape &p,  const shape &q)
{    point n = q.south( );
     point s = p.north( );
     p.move(n.x - s.x, n.y - s.y - 1); }
// Cборная пользовательская фигура – физиономия
class myshape : public rectangle {   // Моя фигура ЯВЛЯЕТСЯ
     int w, h;			             //        прямоугольником
     line l_eye;    // левый глаз – моя фигура СОДЕРЖИТ линию
     line r_eye;   // правый глаз
     line mouth;  // рот
  public:
     myshape(point, point);
     void draw( );
     void move(int, int);
	 void resize(double r) { rectangle::resize(r); rectangle::move(w*(1-r)*0.5, h*(1-r)*0.5); }
	 void rotate_left() {}
	 void rotate_right() {}
};
myshape :: myshape(point a, point b)
	: rectangle(a, b),	//Инициализация базового класса
	  w(neast( ).x - swest( ).x + 1), // Инициализация данных
	  h(neast( ).y - swest( ).y + 1), // – строго в порядке объявления!
	  l_eye(point(swest( ).x + 2, swest( ).y + h * 3 / 4), 3),
	  r_eye(point(swest( ).x + w - 4, swest( ).y + h * 3 / 4), 3),
	  mouth(point(swest( ).x + 2, swest( ).y + h / 4), w - 4) 
{ }
void myshape :: draw( )
{
	 rectangle :: draw( );      //Контур лица (глаза и нос рисуются сами!) 
	 int a = (swest( ).x + neast( ).x) / 2;
	 int b = (swest( ).y + neast( ).y) / 2;
	 /*int a = (swest( ).x + neast( ).x) / 1.2;
	 int b = (swest( ).y + neast( ).y) / 1.2;*/
	 put_point(point(a, b));   // Нос – существует только на рисунке!
}
void myshape :: move(int a, int b)
{
	 rectangle :: move(a, b);
	 l_eye.move(a, b);  r_eye.move(a, b);
	 mouth.move(a, b);
}
cross createCross(const std::string& figureName, const point& c, int s) {
    try {
        return cross(figureName, c, s); 
    } catch (const PointOffScreen& e) {
        std::cerr << e.what() << '\n';
        std::cerr << "Usage of default parameters!" << std::endl;
        return cross("DefaultCross", point(8, 3), 3);
    } catch (const InvalidFigureParameters& e) {
        std::cerr << e.what() << '\n';
        std::cerr << "Usage of default parameters!" << std::endl;
        return cross("DefaultCross", point(8, 3), 3); 
    }
}

int main( ) 
{   setlocale(LC_ALL, "en_US.UTF-8");
	screen_init( );
//== 1. Объявление набора фигур ==
	

    // Création des croix avec gestion centralisée des exceptions et noms spécifiques
    cross myCross1 = createCross("MyCross1", point(120, 3), 3);
    cross myCross2 = createCross("MyCross2", point(34, 3), 3);
    cross myCross3 = createCross("MyCross3", point(24, 9), 3);
    cross myCross4 = createCross("MyCross4", point(12, 6), 3);

	rectangle hat(point(0, 0), point(14, 5));
	//cross my_cross(point(21,5),3);
	line brim(point(20,9),17);
	myshape face(point(15,10), point(27,18));
	h_circle beard(point(40,10), 5);
	shape_refresh( );
	std::cout << "=== Generated... ===\n";
	std::cin.get(); //Смотреть исходный набор
//== 2. Подготовка к сборке ==
	hat.rotate_right( );
	brim.resize(2.0);
	//face.resize(1.2);
	face.resize(1.5);
	beard.flip_vertically( );
	beard.resize(1.5);
    shape_refresh( );
	std::cout << "=== Prepared... ===\n";
	std::cin.get(); //Смотреть результат поворотов/отражений
//== 3. Сборка изображения ==
	
	face.move(0, -8); // Лицо – в исходное положение (если нужно!)
	up(brim, face);
	up(hat, brim);
	down(beard, face);
	left(myCross1, beard);
	right(myCross2, beard);
	push_vert(myCross2, 10);
	push_vert(myCross1, 10);
	push_side(myCross1, -2);
	push_side(myCross2, 2);
	push_side(myCross3, -6);
	push_vert(myCross3, -3);
	push_side(myCross4, 13);
	(myCross1, beard);
	shape_refresh( );
	std::cout << "=== Ready! ===\n";
	std::cin.get();       //Смотреть результат
	
	screen_destroy( );
	return 0; 
}

