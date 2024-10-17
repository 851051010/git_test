#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

// 1.1 auto初始化
void test01()
{
    // 使用auto必须立刻初始化
    //auto x;  错误，必须初始化
    auto x = 10; // x的类型被推导为int
    auto y = 2.5; // y的类型被推导为double
    auto z = 'a'; // z的类型被推导为char
    cout << typeid(x).name() << " " << typeid(y).name() << " " << typeid(z).name() << endl; // 输出类型信息
    cout << sizeof(x) << " " << sizeof(y) << " " << sizeof(z) << endl; // 输出大小信息

    //auto和其他类型变量的混合使用
    int a = 10;
    auto* p1 = &a; // p1的类型被推导为int*,auto是int
    cout << "p1类型: " << typeid(p1).name() << endl; // 输出类型信息
    auto p2 = &a; // p2的类型被推导为int*,auto是int*
    cout << "p2类型: " << typeid(p2).name() << endl; // 输出类型信息
    auto& r1 = a; // r1的类型被推导为int&,auto是int
    cout << "r1类型: " << typeid(r1).name() << endl; // 输出类型信息
    auto r2 = r1; // r1是int&类型，r2应该也是int&类型，但是通过打印发现引用类型会打印它的原始类型
    cout << "r2类型: " << typeid(r2).name() << endl; // 输出类型信息
}
//1.2 auto的使用限制
    //1)auto不能用于定义数组
    //auto arr[] = {1, 2, 3}; // 错误，不能使用auto定义数组

    //2) atuo 不能用于类的成员变量
    class TestAuto
    {
        // auto m_name = "Tom"; // 错误，不能使用auto定义类的成员变量
        int m_age = 10;
    };

    //3) auto不能在函数参数中使用
    //int add(auto a = 0, auto b = 1) { return a + b; }

    //4) auto不能用于函数返回值类型
    auto mySum(int num)
    {
        int sum = 0;
        for (int i = 0; i <= num; i++)
        {
            sum += i;
        }
        return sum;
    }

    //5）auto不能用于模板参数
    template<class T>
    class A
    {
        T m_data;
    };
    A<int> a1;
    A<char> a2;
   // A<auto> a3; // 错误，不能使用auto作为模板参数

// 1.3 auto的使用场景
void test02()
{
    //2）在循环语句中使用auto作为循环的控制变量
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (auto i = 0; i < 10; i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;

    //定义迭代器，使用auto作为迭代器的类型
    vector<int> v = {1, 2, 3};
    //之前的用法
    for (vector<int>::iterator it = v.begin(); it != v.end(); it++)
    {
        cout << *it << " ";
    }
    cout << endl;
    //使用auto
    for (auto it = v.begin(); it != v.end(); it++)
    {
        cout << *it << " ";
    }
    cout << endl;
}

//2.空指针nullptr
void fun(int n) { cout << n << ",调用参数为int的函数" << endl; }
void fun(const char* s) { const char* c = "MmeSweet"; s = c; cout << s << ",调用参数为const char*的函数" << endl; }
void test03()
{
    fun(NULL); //我们发现,NULL当作0处理了，调用的是int参数的函数重载，这样显然不是我们期望的
    fun(nullptr);//换成nullptr，编译器会将nullptr解析成0，调用的是const char*参数的函数重载

    char* p_char = nullptr; 
    int* p_int = nullptr;
    int n = 10;
    //if (n==nullptr) {} // 错误，nullptr不能与整型比较
}

//3.Lambda表达式
void test04()
{
    [] {}; //空的匿名函数,什么都不做
    int a = 10;
    int b = 20;
    [=] {return a + b; }; //值传递捕捉所有外部变量，并返回a+b的和
    //执行匿名函数时，只需在后面加上括号()
    cout << "a + b = " << ([=] {return a + b; })() << endl; //输出结果为30

    //定义一个带参数的匿名函数
    [](int num1, int num2) {return num1 + num2; }; //可以不用定义，直接执行
    cout << [](int num1, int num2) {return num1 + num2; }(10, 20) << endl; //输出结果为30

    //定义带返回值的匿名函数
    cout << [=]()->int {return a + b; }() << endl; //输出结果为30,注意：如果有返回值，不可以省略括号()

    //结合auto来使用匿名函数
    auto f = [] {return 100; }; //定义一个匿名函数，返回值为100
    cout << "f() = " << f() << endl; //输出结果为100
    //使用引用捕捉
    auto f1 = [&](int c) {b = a + c; }; //定义一个带参数的匿名函数，修改b的值
    f1(10);
    cout << "b = " << b << endl; //输出结果为20

    //mutable的使用
    int x = 1;
    int y = 2;
    //auto add1 = [x, y]()->int {x *= 2; return x + y; }; //报错，x具有常性，不能修改
    auto add1 = [x, y]()mutable->int {x *= 2; return x + y; }; //mutable取消了常性，可以修改x的值
    cout << "add1() = " << add1() << endl; //输出结果为4

    //匿名函数也可以在函数内部调用普通函数
    auto  fun_mySum = []()->int {return mySum(10); };
    cout << fun_mySum() << endl;
}
void test05()
{
    //课堂练习：使用匿名函数代替SIL中的排序器函数，实现排序算法的倒叙排序，操作一个vector<int>
    vector<int> v = { 2,34,5,3,7 };
    for (auto i : v)
    {
        cout << i << " ";
    }
    cout << endl;
    //排序算法的倒叙排序
    sort(v.begin(), v.end(), [&](int a, int b) {return a > b; });
    for (auto i : v)
    {
        cout << i << " ";
    }
    cout << endl;
}

//4.for范围遍历
void test06()
{
    //传统遍历方式
    int arr[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    for (int i = 0; i < sizeof(arr)/sizeof(arr[0]); i++)
    {
        cout << arr[i] << " ";
    }
    cout << endl;

    char arr_c[] = "hello world";
    for (int i = 0; i < strlen(arr_c); i++)
    {
        cout << arr_c[i];
    }
    cout << endl;

    // for范围遍历
    for (auto i : arr)
    {
        cout << i << " ";  //这里直接输出i就可以，不需要使用arr[i]输出
    }
    cout << endl;
    for (auto c : arr_c)
    {
        cout << c;
    }
    cout << endl;

    //遍历容器
    vector<int> v = { 1,2,3,4,5 };
    for (auto i : v)
    {
        cout << i << " ";
    }
    cout << endl;
}
//通过for循环遍历，实现容器的模板函数，打印vector容器和map容器的模板
template<typename T>
void printVector(vector<T>& v)
{
    for (auto ele : v)
    {
        cout << ele << " ";
    }
    cout << endl;
}
template<typename T1,typename T2>
void printMap(map<T1,T2>& m)
{
    for (auto ele : m)
    {
        cout << ele.first << ":" << ele.second << endl;
    }
}
void test07()
{
     //测试printVector函数
    vector<int> v1 = { 1,2,3,2,5,8 };
    vector<string> v2 = {"Tom","Jerry","Mike"};
    printVector(v1);
    printVector(v2);

    //测试printMap函数  
    map<string, int> m1 = { {"Tom",10},{"Jerry",20},{"Mike",30} };
    map<int, int> m2 = { {1,10},{2,20},{3,30} };
    printMap(m1);
    printMap(m2);

    for (auto num : {1,23,34,4,52,46}) {cout << num << " ";}
    cout << endl;

    // for范围遍历字符串的时候，会把字符串的结束符也遍历
    int count = 0;
    char arr_char[] = "hello";
    for (int i = 0; i < strlen(arr_char); i++) //strlen函数不统计结束符\0
    {
        count++;
    }
    cout << "strlen count = " << count << endl; //输出结果为5

    //for范围遍历字符串
    count = 0;
    for (char c : arr_char)
    {
        count++;
    }
    cout << "for count = " << count << endl; //输出结果为6,会统计结束符
}
//for范围遍历的引用遍历
void test08()
{
    vector<int> v1 = { 1,2,3,4,5,6 };
    for (auto& i : v1){cout << ++i << " ";} //引用的方式遍历，每次都+1后再输出
    cout << endl;
    //操作完之后，vector中的值就被改变了
    for (auto& i : v1) { cout << i << " "; }
}

//5.初始化列表
//结构体
struct Point
{
    int px;
    int py;
    Point(int x, int y) :px(x),py(y) {}
};
//类
class Date
{
public:
    int m_Year;
    int m_Month;
    int m_Day;
    Date(int y,int m,int d) :m_Year(y),m_Month(m),m_Day(d) {}
};
void test09()
{
    int arr1[] = { 4,5,6,7,8 };
    int arr2[5] = { 8 }; //5个8
    //自定义类型初始化
    Point p = { 10,20 };
    cout << p.px << "，" << p.py << endl;
    Date d1 = { 2024,10,14 };
    Date d2{ 2024,10,15 }; //可以省略花括号
    cout << d1.m_Year << "-" << d1.m_Month << "-" << d1.m_Day << endl;
    cout << d2.m_Year << "-" << d2.m_Month << "-" << d2.m_Day << endl;
    //容器的初始化
    vector<int> v1 = { 1,2,3,4,5 };
    vector<Date> v2 = { {2024,10,14}, {2024,10,15} }; 
    //申请堆内存的时候初始化
    Point* pp = new Point[3]{ {10,20}, {30,40}, {50,60} };
    for (int i = 0; i < 3; i++)
    {
        cout << (pp + i)->px << "，" << (pp + i)->py << endl;
    }
    int* p_int = new int[5] { 1, 2, 3, 4, 5 };
    for (int i = 0; i < 5; i++)
    {
        cout << *(p_int + i) << " ";
    }
    cout << endl;
    //释放堆内存
    delete[] pp;
    delete[] p_int;

    //初始化列表的类型,可见视始化列表是用模板实现的，所以可以对不同类型进行初始化
    auto ls = { 1,2,3 };
    cout << typeid(ls).name() << endl; //输出类型信息
    auto cls = { 'a','b','c' };
    cout << typeid(cls).name() << endl; //输出类型信息
}


//6.类型别名
void test10()
{
    typedef Date D; //自定义类型起别名
    typedef double dou; //基本类型起别名
    using P = Point;  //using的语法

    //使用别名
    D d1 = { 2024,10,16 }; 
    dou d2 = 3.14;
    P p1 = { 2,1 };
}

//7.右值引用和move（移动）语义
void test11()
{
    int n = 3;
    int& a = n; //左值引用
    // int& b = 5; //错误，左值引用无法引用右值(可以用const修饰，这样就可以引用了)
    int&& b = 5; //右值引用
    cout << &"hello" << endl; //字符串字面量是左值，可以取地址
    //cout << &3 << endl; //3是右值，不能取地址


    int n1 = 1;
    int n2 = 2;
    //cout << &(n1 + n2) << endl; //表达式计算的结果是临时的。是右值，不能取地址
    int res = n1 + n2;
    cout << &res << endl; // res保存了n1+n2的结果，是一个左值，可以取地址
}

class Student
{
    string name;
    int age;
public:
    Student(string n, int a) :name(n), age(a) {};
    void show() {cout << "name:" << name << " age:" << age << endl; }
};
void test12()
{
    Student s1("Tao", 21);
    cout << "s1:" ;
    s1.show();
    Student s2 = s1; //拷贝构造函数
    cout << "s2:" ;
    s2.show();
    Student s3(move(s1)); //移动构造函数,取得s1的所有权，此时s3就相当于s1，这时候s1就被析构了
    cout << "s3:" ;
    s3.show();
    cout << "s1:" ;
    s1.show(); //这是name的值就没有了，因为s1已经被析构了。age还有值是因为是基本类型，没有被移动

    Student s4 = s2; //赋值函数
    cout << "s4:" ;
    s4.show();
    Student s5 = move(s2); //move将s2转换成右值，然后使用了移动赋值函数，s5就相当于s2，这时候s2就被析构了
    cout << "s5:" ;
    s5.show();
    cout << "s2:";
    s2.show();

    //在容器中使用移动语义
    vector<string> v1 = { "Mme","Tao","Sweet","WYMM"};
    vector<string> v2 = v1; //拷贝构造函数
    vector<string> v3 = move(v1); //通过move函数转换成右值，实现调用移动赋值函数，提高效率，避免了复制的过程

    vector<vector<int>> res;
    vector<int> temp;
    temp.push_back(2); //添加基本类型，没必要使用move，因为基本类型无法优化
    res.push_back(move(temp)); //这里使用移动就可以提供更高的效率
}

int main()
{
    test01();
    cout << endl;
    test02();
    cout << endl;
    test03();
    cout << endl;
    test04();
    cout << endl;
    test05();
    cout << endl;
    test06();
    cout << endl;
    test07();
    cout << endl;
    test08();
    cout << endl;
    test09();
    cout << endl;
    test11();
    cout << endl;
    test12();
    cout << endl;
    return 0;
}

/*
C++11新特性：
    1.自动类型推导
    2.空指针nullptr
    3.Lambda表达式
    4.for范围遍历
    5.初始化列表
    6.类型别名
    7.右值引用和move（移动）语义
    8.智能指针
    9.thread多线程

1.自动类型推导：
    1.1 auto初始化
        在C++11之前，auto就存在，它用来指定变量的存储类型，它是跟static关键字相对的。代表自动存储。这是编译器默认的规则，所以一般我们都省略不写
        C++11之后，auto被赋予新的含义，用来做变量类型的自动推导。也就是说，使用了auto之后，我们不用指定变量的类型，而是让编译器自动推导变量的类型
        语法：auto var = expression;  此时auto起到了一个占位的作用，等到编译器编译到此处时，会根据expression的类型来确定var的类型。
        例子：auto x = 10; // x的类型为int
        注意事项：使用auto的时候，由于需要根据变量的值来推导具体的类型，所以必须对变量进行初始化，否则编译器无法推导变量的类型。
    1.2 auto的使用限制
        1）使用auto的时候必须对变量进行初始化。除此之外，auto不能用于定义数组。
        2）auto不能用于类的成员变量。
        3）auto不能在函数参数中使用，因为形参只有在函数调用的时候才会被赋值初始化。
        4）auto不能用于函数返回值类型，可以用于函数的定义，但是不能用于函数的声明。
        5）auto不能用于函数模板参数。
    1.3 auto的使用场景
        1) 直接在定义变量时，使用auto关键字
        2）在循环语句中使用auto作为循环的控制变量，比如定义迭代器，使用auto作为迭代器的类型
        
2.空指针nullptr
    空指针NULL，在C++中除了NULL，0也可以作为空指针，所以NULL会被当作0处理
    这种情况，在某些函数调用的时候会存在问题。如果一个函数有重载，重载的是两个不同的参数，一个参数是指针类型，一个参数是整型。这个时候，当我们传入
    NULL的时候，NULL会被解析成0，从而调用int参数的那个函数，而这显然是不对的。
    为了解决这个问题，引入了nullptr来充当空指针
    注意：nullptr不能跟整型比较，也不能转化成整型

3.Lambda表达式
    匿名函数，是一种函数，但是没有名字，可以直接使用。
    语法：[捕捉列表](参数列表)mutable（可选）->返回值类型{函数体}
        [捕捉列表]：它一定是出现在匿名函数的最开始位置，不能少。编译器根据[]判断是匿名函数。它的作用是用来捕捉当前作用域中的变量，供匿名函数使用
                    通过捕捉列表来确定匿名函数在调用的时候，可以使用哪些外部变量。
                    捕捉列表常见用法：
                        []：匿名函数不捕获任何外部变量。（如果是全局变量或者静态变量，匿名函仍然可以使用）
                        [变量a,变量b,变量c...]：以传值的方式使用多个外部的变量（值捕捉过来的变量具有常量特性，不能修改，也就是说不能在匿名函数中修改这些变量）
                        [变量&a,&b,&c...]：以引用的方式使用多个外部的变量（引用捕捉过来的变量具有可修改特性，可以在匿名函数中修改这些变量）
                        [=]：捕捉所有外部变量，以传值的方式使用，同样具有常性。
                        [&]：捕捉所有外部变量，以引用的方式使用，同样具有可修改性。

                        
        (参数列表)：它跟普通函数用法相同。如果匿名函数没有参数，()可以省略。但是如果指定了返回值的时候->,()必须要写。参数没有常性，可以修改
        mutable：可选，默认情况下，值捕捉的变量是常量，不能修改。但是加上mutable之后，就可以在匿名函数中修改这个变量的值。但是无法修改原值，只能修改副本。
                 除非是引用捕捉，才可以修改原值。同时，如果加了mutble，()也不能省略，即使为空也不能省略。
        ->返回值类型：可选，如果匿名函数没有返回值，可以省略。或者返回值明确的情况下，也可以不写。如果有返回值，则必须要写。
        {函数体}：匿名函数的主体，可以写任意代码。这里面可以使用捕捉过来的变量，也可以使用自己的参数和自己定义的变量。

4.for范围遍历
    这是一种新的遍历方式，可以对一个可便利的序列进行逐个遍历，但是它无法指定遍历的范围，只能遍历整个序列。如果需要指定遍历的范围，仍然使用传统的for循环
    for范围遍历语法：for(变量类型 变量名:可便利的序列) {循环体语句}
        变量类型：要和序列中的元素类型一致。可以是auto，也可以是具体的类型。
        可便利的序列：可以是数组、容器，也可以是迭代器或者一个大括号初始化的序列。
     for范围遍历的引用语法：for(变量类型& 变量名:可便利的序列) {循环体语句}  //引用遍历可以修改序列中的原值

5.初始化列表
    使用{}初始化
    在C++11之前，就可以使用大括号对数组进行初始化
    C++11之后，扩大了它的适用范围，可以对所有的内置类型和用户自定义类型进行初始化。

6.类型别名
    类型别名就是给类型(基本类型和自定义类型)起别名，然后就可以使用别名，等价于原类型名
    1)typedef 类型原名 类型别名;
    2)using 类型别名 = 类型原名;
    以上两种方式只有一个区别，那就是typedef这种方式不能用于给模板类起别名，其他方面它们两个等价

7.右值引用和move（移动）语义
    右值引用也是C++11的重要特性。它主要解决性能的问题，可以免除某些场景下的一些额外的性能开销
    左值和右值：
        左值：可以放到等号左边的值，可以取地址，并且有名字。比如说函数名、变量名、引用、指针、返回左值引用的函数调用、字符串字面量如"hel1o"等。
        右值：只能放到等号右边的值，不能取地址，没有名字。比如说除字符串以外的字面量、非引用返回的临时变量、表达式产生的临时结果、匿名函数等。
        举例： int a = 5; // a是左值，可以通过&取地址，并且有名字，而5是右值，不能取地址，没有名字。
    右值引用：
        左值引用是对左值的引用或者叫做别名，右值引用是对右值的引用或者叫做别名。语法上，就是左值引用基础上多加一个引用符号&。
        右值引用的声明：类型&& 变量名 = 表达式;
    为什么要引入右值引用
        右值引用主要为了解决性能问题，C++11之前程序运行过程中会产生大量的临时对象，主要有以下几个方面：
            函数的返回值
                最常见是就是返回自定义类型，如果没有用对象接受，就会产生临时对象。比如有个函数返回一个临时对象，但是没有使用它，
                这时候编译器就会创建一个临时对象来存储这个返回值。而且将来还会调用析构函数将它析构掉，当有大量的临时对象产生的时候，或者临时对象占用的资源
                很大的时候，就会造成性能问题。
            计算结果
                一些表达式的计算结果也是临时对象
            值传递的形参
                比如有一个函数值传递一个对象参数，值传递的过程其实就是做了一个拷贝构造，进行复制。这时也会产生临时对象
        C++11之后，有了右值引用，就可以解决上述问题。当我们不希望产生临时对象的时候，可以使用移动函数move来将一个对象变成右值，通过
        移动构造的方式，或者通过移动赋值的方式，取得这个对象的所有权，从而避免产生临时对象。

        关于移动构造和移动赋值：
            C++11之前，一个自定义类型（结构体或者类）如果我们没有手动声明，编译器会默认生成四个函数：
            构造函数、拷贝构造函数、赋值运算符重载函数、析构函数
            C++之后，编译器会多生成两个函数：分别是移动构造函数、移动赋值函数。这样我们就可以通过移动的方式，取得对象的所有权，而不用去复制
            注意：移动语义只对自定义类型有效，对于基本类型，移动语义没有意义。
            STL中的容器类几乎都是支持移动语义的，这样就可以通过move转换成右值后，调用移动函数进行移动，避免拷贝的动作。来提高性能
            还有一些特殊的类，它们只允许移动不允许拷贝，比如unique_ptr、thread等。

            右值引用注意事项：
                1）右值引用和左值引用一样都需要马上初始化
                2）右值引用无法指向左值，这时候可以使用move函数将它转成右值
                3）对于基本类型，没必要使用move进行移动，因为基本类型的拷贝开销很小。move函数只是将左值转换成右值，但能不能移动主要看自定义类型中有没有实现移动构造和移动赋值
                4）移动之后的对象会被析构，所以通常是对一些临时对象进行移动，或者是对不再使用的对象进行移动。如果还需要使用的对象，就不要移动了
                5）使用const修饰的左值引用也可以引用右值，但是无法修改
*/
