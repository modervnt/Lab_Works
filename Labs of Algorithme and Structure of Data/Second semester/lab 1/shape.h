#include <list>
using std::list;
//==1. Поддержка экрана в форме матрицы символов ==
char screen[YMAX] [XMAX];	
enum color { black = '*', white = '.' };
void screen_init( )
{
  for (auto y = 0; y < YMAX; ++y)
    for (auto &x : screen[y])  x = white;
}
void screen_destroy( )
{
  for (auto y = 0; y < YMAX; ++y)
    for (auto &x : screen[y])  x = black;
}
bool on_screen(int a, int b) // проверка попадания точки на экран
{ return 0 <= a && a < XMAX && 0 <= b && b < YMAX; }
void put_point(int a, int b)
{ if (on_screen(a,b)) screen[b] [a] = black; }
void put_line(int x0, int y0, int x1, int y1)
/* Алгоритм Брезенхэма для прямой:
рисование отрезка прямой от (x0, y0) до (x1, y1).
Уравнение прямой: b(x–x0) + a(y–y0) = 0.
Минимизируется величина abs(eps), где eps = 2*(b(x–x0)) + a(y–y0).  */
{
  int dx = 1;
  int a = x1 - x0;   if (a < 0) dx = -1, a = -a;
  int dy = 1;
  int b = y1 - y0;   if (b < 0) dy = -1, b = -b;
  int two_a = 2*a;
  int two_b = 2*b;
  int xcrit = -b + two_a;
  int eps = 0;
  for (;;) { //Формирование прямой линии по точкам
	  put_point(x0, y0);
	  if (x0 == x1 && y0 == y1) break;
	  if (eps <= xcrit) x0 += dx, eps += two_b;
	  if (eps >= a || a < b) y0 += dy, eps -= two_a;
  }
}
void screen_clear( ) { screen_init( ); } //Очистка экрана
void screen_refresh( ) // Обновление экрана
{
  for (int y = YMAX-1; 0 <= y; --y) { // с верхней строки до нижней
    for (auto x : screen[y])                 // от левого столбца до правого
      std::cout << x;
    std::cout << '\n';
  }
}
//== 2. Библиотека фигур ==
struct shape {   // Виртуальный базовый класс «фигура»
  static list<shape*> shapes;       // Список фигур (один на все фигуры!)
  shape( ) { shapes.push_back(this); } //Фигура присоединяется к списку
  virtual point north( ) const = 0;  //Точки для привязки
  virtual point south( ) const = 0;
  virtual point east( ) const = 0;
  virtual point west( ) const = 0;
  virtual point neast( ) const = 0;
  virtual point seast( ) const = 0;
  virtual point nwest( ) const = 0;
  virtual point swest( ) const = 0;
  virtual void draw( ) = 0;		//Рисование
  virtual void move(int, int) = 0;	//Перемещение
  virtual void resize(double) = 0;    	//Изменение размера
  virtual ~shape( ) { shapes.remove(this); } //Деструктор
};
list<shape*> shape::shapes;   // Размещение списка фигур
void shape_refresh( )    // Перерисовка всех фигур на экране
{
  screen_clear( );
  for (auto p : shape :: shapes) p->draw( ); //Динамическое связывание!!!
  screen_refresh( );
}

class rotatable : virtual public shape { 	//Фигуры, пригодные к повороту 
protected:
	enum class rotated { left, no, right };
	rotated state;           //Текущее состояние поворота
public:
	rotatable(rotated r = rotated::no) : state(r) { }
	void rotate_left() { state = rotated::left; }
	//Повернуть влево
	void rotate_right() { state = rotated::right; }
	//Повернуть вправо
};
class reflectable : virtual public shape { 	//Фигуры, пригодные к зеркальному отражению
protected:
	bool hor, vert;         //Текущее состояние отражения
public:
	reflectable(bool h = false, bool v = false) : hor(h), vert(v) { }
	void flip_horisontally() { hor = !hor; }		//Отразить горизонтально
	void flip_vertically() { vert = !vert; } 	//Отразить вертикально
};

class line : public shape {        // ==== Прямая линия ====
/* отрезок прямой ["w", "e"].
   north( ) определяет точку «выше центра отрезка и так далеко
   на север, как самая его северная точка», и т. п. */
protected:
	point w, e;
public:
  line(point a, point b) : w(a), e(b) { }; //Произвольная линия (по двум точкам)
  line(point a, int L) : w(point(a.x + L - 1, a.y)), e(a) {  }; //Горизонтальная линия
  point north( ) const { return point((w.x+e.x)/2, e.y<w.y? w.y : e.y); }
  point south( ) const { return point((w.x+e.x)/2, e.y<w.y? e.y : w.y); }
  point east( ) const { return point(e.x<w.x? w.x : e.x, (w.y+e.y)/2); }
  point west( ) const { return point(e.x<w.x? e.x : w.x, (w.y+e.y)/2); }
  point neast( ) const { return point(w.x<e.x? e.x : w.x, e.y<w.y? w.y : e.y); }
  point seast( ) const { return point(w.x<e.x? e.x : w.x, e.y<w.y? e.y : w.y); }
  point nwest( ) const { return point(w.x<e.x? w.x : e.x, e.y<w.y? w.y : e.y); }
  point swest( ) const { return point(w.x<e.x? w.x : e.x, e.y<w.y? e.y : w.y); }
  void move(int a, int b) 	{ w.x += a; w.y += b; e.x += a; e.y += b; }
  void draw( ) { put_line(w, e); }
  void resize (double d)                // Изменение длины линии в (d) раз
  { e.x = w.x + (e.x - w.x) * d; e.y = w.y + (e.y - w.y) * d; }
};

    
class rectangle : public rotatable {      // ==== Прямоугольник ====
/* nw ------ n ------ ne
   |		       |
   |		       |
   w	   c            e
   |		       |
   |		       |
   sw ------- s ------ se */
protected:
  point sw, ne;
public:
  rectangle(point a, point b) :  sw(a), ne(b) { }
  point north( ) const { return point((sw.x + ne.x) / 2, ne.y); }
  point south( ) const { return point((sw.x + ne.x) / 2, sw.y); }
  point east( ) const { return point(ne.x, (sw.y + ne.y) / 2); }
  point west( ) const { return point(sw.x, (sw.y + ne.y) / 2); }
  point neast( ) const { return ne; }
  point seast( ) const { return point(ne.x, sw.y); }
  point nwest( ) const { return point(sw.x, ne.y); }
  point swest( ) const { return sw; }
  void rotate_right( )           // Поворот вправо относительно se
	{ int w = ne.x - sw.x, h = ne.y - sw.y; // (учитывается масштаб по осям)
	  sw.x = ne.x - h * 2; ne.y = sw.y + w / 2;	}
  void rotate_left() // Поворот влево относительно sw
	{ int w = ne.x - sw.x, h = ne.y - sw.y; 
	  ne.x = sw.x + h * 2; ne.y = sw.y + w / 2; }
  void move(int a, int b)
	{ sw.x += a; sw.y += b; ne.x += a; ne.y += b; }
  void resize(int d) 
  { ne.x = sw.x + (ne.x - sw.x) * d; ne.y = sw.y + (ne.y - sw.y) * d; }
  void resize(double d)
	{
	  ne.x = sw.x + (ne.x - sw.x) * d;    ne.y = sw.y + (ne.y - sw.y) * d;
	}
  void draw( )
  { 
    put_line(nwest( ), ne);   put_line(ne, seast( ));
    put_line(seast( ), sw);   put_line(sw, nwest( ));
  }
};

class cross : public shape { 
  protected:
      point center;  
      int size;      
  
  public:
      // Constructeur
      cross(point c, int s)
          : center(c), size(s) { }
  
      // Redéfinition des méthodes virtuelles pures
      point north() const override { return point(center.x, center.y - size / 2); }
      point south() const override { return point(center.x, center.y + size / 2); }
      point east() const override { return point(center.x + size / 2, center.y); }
      point west() const override { return point(center.x - size / 2, center.y); }
      point neast() const override { return point(center.x + size / 2, center.y - size / 2); }
      point seast() const override { return point(center.x + size / 2, center.y + size / 2); }
      point nwest() const override { return point(center.x - size / 2, center.y - size / 2); }
      point swest() const override { return point(center.x - size / 2, center.y + size / 2); }
  
      void draw() override {
          put_line(nwest(), seast()); 
          put_line(neast(), swest()); 
      }

      void move(int dx, int dy) override {
          center.x += dx;
          center.y += dy;
      }

 
      void resize(double factor) override {
          size = static_cast<int>(size * factor);
      }
  };

void up(shape& p, const shape& q) // поместить фигуру p над фигурой q
{	//Это ОБЫЧНАЯ функция, не член класса! Динамическое связыва-ние!!
  point n = q.north( );
  point s = p.south( );
  p.move(n.x - s.x, n.y - s.y + 1);
}

void push_vert(shape& p,int d )
{
  p.move(0, d);
}

void push_side(shape& p,int d )
{
  p.move(d, 0);
}



void right(shape &p,  const shape &q)
{    point n = q.seast( );
    point s = p.west( );
    p.move(n.x - s.x + 2, n.y - s.y ); }

void left(shape &p,  const shape &q)
{    point n = q.swest( );
    point s = p.east( );
    p.move(n.x - s.x -2, n.y - s.y ); }

