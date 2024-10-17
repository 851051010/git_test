#include <iostream>
#include <vector>
#include <memory>
using namespace std;

//模拟智能指针类
template <class T>
class SmartPtr
{
	//先封装一个裸指针
	T* ptr;
public:
	//构造函数，传入一个裸指针
	SmartPtr(T* p) :ptr(p) {}
	//析构函数，释放资源
	~SmartPtr() { if (ptr != nullptr) { delete ptr; cout << "智能指针完成释放" << endl; } }
};
//使用智能指针
void test01()
{
	SmartPtr<int> ptr_int(new int(100));
	SmartPtr<string> ptr_str(new string("hello"));
}

//定义一个类型给智能指针使用
class Test
{
	int m_num;
public:
	Test() {cout << "Test类的无参构造函数" << endl; }
	Test(int x) : m_num(x) {cout << "Test类的有参构造函数，参数：" << x << endl; }
	~Test() {cout << "Test类的析构函数"<< endl; }
	void setValue(int v) { m_num = v; }
	void print() { cout << "m_num = " << m_num << endl; }
};
//测速智能指针
void test02()
{
	//构造
	shared_ptr<int> ptr(new int(1));
	cout << ptr << endl;
	cout << *ptr << endl;//智能指针可以像普通指针一样使用运算符
	*ptr = 30; //智能指针可以像普通指针一样修改值
	cout << *ptr << endl;

	//拷贝构造完成共享
	shared_ptr<int> ptr1(ptr);//此时它们共同管理同一块内存
	cout << ptr1 << endl;
	cout << *ptr1 << endl;

	//辅助函数make_shared(效率更高一些)
	shared_ptr<int> ptr2 = make_shared<int>(10);
	cout << *ptr2 << endl;
	shared_ptr<int> ptr3 = make_shared<int>(); //管理一个没有初始化的空间
	
	//查看引用计数
	cout << ptr.use_count() << endl; //引用计数为2
	cout << ptr1.use_count() << endl; //引用计数为2
	cout << ptr2.use_count() << endl; //引用计数为1
	cout << ptr3.use_count() << endl; //引用计数为1

	//赋值函数
	ptr3 = ptr2; //它俩共同管理同一块内存，即ptr2的内存，就是10
	cout << *ptr3 << endl; //输出10

	//重置
	ptr3.reset(); //不传参，不是唯一管理者，引用计数-1，ptr3置空
	cout << ptr2.use_count() << endl; //引用计数为1
	cout << ptr3.use_count() << endl; //引用计数为0,ptr3被重置了，不管理任何内存
	ptr2.reset();
	cout << ptr2.use_count() << endl; //引用计数为0,ptr2被重置了，不管理任何内存
	ptr1.reset(new int(5)); //ptr1被重置，同时管理新内存5
	cout << *ptr1 << endl; //输出5
	cout << ptr1.use_count() << endl; //引用计数为1
	cout << ptr.use_count() << endl; //引用计数为1

	//使用get方法
	shared_ptr<Test> ptr4(new Test(999));
	shared_ptr<Test> ptr5 = make_shared<Test>(888);
	Test* t1 = ptr4.get();
	t1->print(); //输出999
}

//手动指定删除动作，也叫做删除器
void test03()
{
	//智能指针无法全部释放连续的多个自定义类型的空间
	//shared_ptr<Test> ptr_list(new Test[3]); //这是一个连续的内存空间，保存了3个Test对象,析构的时候智能析构一次，所以会报错
	//对于基本类型的连续空间，智能指针可以做到全部释放
	shared_ptr<int> ptr_int_list(new int[3]);

	//接下来通过手动指定删除器来完成连续空间的释放
	//1.在构造的时候，传入一个匿名函数来实现连续空间的释放
	shared_ptr<Test> ptr1(new Test[3], [](Test* p) {delete[] p; p = nullptr; });
	//2.使用系统提供的default_delete函数代替刚刚的匿名函数
	shared_ptr<Test> ptr2(new Test[3],default_delete<Test[]>());
	//3.更简单的方式
	shared_ptr<Test[]> ptr3(new Test[3]); //这样智能指针会自动调用default_delete函数来释放内存,推荐的方式

}

//共享智能指针使用注意事项
void test04()
{
	//1)不能直接new一个空间给智能指针，而是要使用构造函数或者辅助来管理空间
	//shared_ptr<int> ptr1 = new int(10); //错误，不能直接new一个空间给智能指针
	shared_ptr<Test> ptr1(new Test[2]);
	shared_ptr<Test> ptr2 = make_shared<Test>(100);

	//2)可以通过一个普通指针来初始化智能指针，但是这样空间就交给智能指针管理了，就不能在通过普通指针释放空间了
	Test* pTest = new Test(100); //普通指针
	shared_ptr<Test> pShare(pTest); //交给智能指针管理
	//delete pTest; //这里的操作不能出现，因为智能指针也会释放这块内存，所以会造成多次释放的错误
	//pTest = nullptr;

	//3)不用用一个普通指针（裸指针）初始化多个智能指针，造成了多次释放
	int* p_int = new int(6);
	//shared_ptr<int> p_share1(p_int); //这里会被释放一次
	//shared_ptr<int> p_share2(p_int); //这里也会被释放一次
	//因为以上两个智能指针就没有实现共享，它们都管理着同一块内存，当它们都被销毁的时候，这块内存就被释放了两次，造成了内存泄露。
	//正确的做法是，只用一个普通指针来初始化智能指针，然后让多个智能指针共同管理这块内存。
}

//5）禁止在类的成员函数返回管理this指针的智能指针。
class ABC
{
	int m_num;
public:
	ABC() {cout << "ABC类的无参构造函数" << endl; }
	~ABC() {cout << "ABC类的析构函数" << endl; }
	shared_ptr<ABC> getP() { return shared_ptr<ABC>(this); } //错误，不能返回管理this指针的智能指针
};
void test05()
{
	shared_ptr<ABC> sp(new ABC);
	//sp->getP(); //错误，不能返回管理this指针的智能指针
	//sp会将ABC对象释放一次，getP函数里酷的智能指针对象也会释放一次，因为它管理了this指针，this就是当前对象
}

//6）避免两个类循环引用
//下面两个类循环引用
class B; //前向声明
class A
{
public:
	shared_ptr<B> bsp; //A类中的属性，管理B类的智能指针
	~A() { cout << "A类的析构函数" << endl; }
};
class B
{
public:
	shared_ptr<A> asp; //B类中的属性，管理A类的智能指针
	~B() { cout << "B类的析构函数" << endl; }
};
void test06()
{
	//循环引用
	shared_ptr<A> a(new A); //a管理了a对象
	cout << a.use_count() << endl; //输出1
	shared_ptr<B> b(new B); //b管理了b对象
	cout << b.use_count() << endl; //输出1
	//下面循环引用
	a->bsp = b;//它俩实现了共享
	b->asp = a;//它俩也实现了共享
	//a和b互相引用，导致它们都无法被释放，造成内存泄露
	//共享会导致引用计数增加
	cout << a.use_count() << endl; //输出2
	cout << b.use_count() << endl; //输出2
	//引用计数编程2之后，就会导致内存泄露，因为计数不会变为0了，就不会释放空间，执行可以发现没有调用析构函数
}

//7）容器中会单独拷贝一份
//写一个函数，将一个Test类对象用智能指针管理，然后将智能指针放入容器中
void p_shared_vec(vector<shared_ptr<Test>>& vec, Test* t)
{
	shared_ptr<Test> p_Test(t); //将Test对象用智能指针管理
	vec.push_back(p_Test); //再将智能指针放入容器中
	cout << "当前计数：" << p_Test.use_count() << endl; //输出2
}
vector<shared_ptr<Test>> vec; //全局的一个容器
void test07()
{
	Test* t1 = new Test(10);
	Test* t2 = new Test(20);
	Test* t3 = new Test(30);
	//调用函数
	p_shared_vec(vec, t1);
	p_shared_vec(vec, t2);
	p_shared_vec(vec, t3);
}//这里函数结束后，并不会释放内存，因为容器中还有一份智能指针


//独享智能指针
void test08()
{
	unique_ptr<Test> up1(new Test(10));
	unique_ptr<Test> up2 = make_unique<Test>(21);
	up1->print(); //输出10
	up2->print(); //输出21
	(*up1).print(); 

	//get()函数获取裸指针
	Test* tp = up1.get();
	tp->print(); //输出10

	//reset()函数重置指针
	up1.reset();
	//(*up1).print();  因为被重置了，这样会有异常
	up2.reset(new Test(12)); //重置指针指向新的内存地址
	(*up2).print(); //输出12
}


//弱引用(共享)智能指针
void test09()
{
	shared_ptr<int> sp(new int(6)); //第一个管理者
	cout << "sp.use_count():" << sp.use_count() << endl; //输出1
	weak_ptr<int> wp1;
	cout << "wp1.use_count():" << wp1.use_count() << endl; //输出0
	weak_ptr<int> wp2(sp); //此时wp2监视了sp管理的内存
	cout << "wp2.use_count():" << wp2.use_count() << endl; //输出1
	weak_ptr<int> wp3(sp); //此时wp3也监视了sp管理的内存
	cout << "wp3.use_count():" << wp3.use_count() << endl; //输出1
	
	//增加管理者
	shared_ptr<int> sp1 = sp; //增加一个管理者,这时有两个管理者
	cout << "sp1.use_count():" << sp.use_count() << endl; //输出2
	cout << "wp3.use_count():" << wp3.use_count() << endl; //输出2

	//拷贝构造和赋值函数
	wp1 = wp2; //此时wp1也监视了wp2管理的内存
	weak_ptr<int> wp4(wp3); //此时wp4也监视了wp3管理的内存
	cout << "wp1.use_count():" << wp1.use_count() << endl; //输出2
	cout << "wp4.use_count():" << wp4.use_count() << endl; //输出2

	//查看监视的内存状态，有没有被释放
	cout << wp3.expired() << endl; //输出0(false)
	wp3.reset(); //释放wp3管理的内存
	cout << "wp3.use_count():" << wp3.use_count() << endl; //输出0
	cout << "wp2.use_count():" << wp2.use_count() << endl; //输出2
	shared_ptr<int> sp2 = wp2.lock(); //锁定了一个共享指针对象,然后赋值给sp2，此时共有3个管理者
	cout << *sp2 << endl; //输出6
	cout << "wp2.use_count():" << wp2.use_count() << endl; //输出3
	sp.reset(); //释放sp管理的内存
	sp1.reset(); //释放sp1管理的内存
	sp2.reset(); //释放sp2管理的内存
	cout << "wp2.use_count():" << wp2.use_count() << endl; //输出0
	cout << wp3.expired() << endl; //输出1(true),内存已经释放了
}

//通过弱共享指针解决循环引用问题,只需要将其中一个类的共享指针改成弱共享指针就可解决
class B_break; //前向声明
class A_break
{
public:
	shared_ptr<B_break> bsp; 
	~A_break() { cout << "A_break类的析构函数" << endl; }
};
class B_break
{
public:
	weak_ptr<A_break> asp;  //这里改成弱共享指针，这样就不会增加引用计数，就可以打破循环引用，正常释放内存
	~B_break() { cout << "B_break类的析构函数" << endl; }
};
void test10()
{
	//循环引用
	shared_ptr<A_break> a(new A_break); 
	cout << a.use_count() << endl; //输出1
	shared_ptr<B_break> b(new B_break); 
	cout << b.use_count() << endl; //输出1
	//下面循环引用
	a->bsp = b;//它俩实现了共享
	b->asp = a;//它俩也实现了共享
	cout << a.use_count() << endl; //输出1
	cout << b.use_count() << endl; //输出2
	//这样就可以正常释放了
}


int main()
{
	test01();
	cout << endl;
	test02();
	cout << endl;
	test03();
	cout << endl;
	//test04();
	//cout << endl;
	test05();
	cout << endl;
	test06();
	cout << endl;
	test07(); //这里函数结束后，并不会释放内存，因为容器中还有一份智能指针
	for (auto it = vec.begin(); it != vec.end(); it++)
	{
		cout << it->use_count() << endl; //输出1
	}
	vec.clear();//清空容器,其内部的智能指针也会被释放
	cout << endl;
	test08();
	cout << endl;
	test09();
	cout << endl;
	test10();
	cout << endl;
	return 0;
}

/*
智能指针
	1.内存泄露
		内存泄漏不是指内存消失了，而是由于疏忽或者程序错误，导致程序分配的内存不能够及时释放,从而无法再被使用的问题，主要指的是堆内存。
		内存泄露如果长期出现，会导致程序越来越慢，最终卡死，对于C++这种需要自己管理内存的语言来说，内存泄露是尤其需要注意的。
		内存泄露不仅仅是堆内存的泄露，还有操作系统资源的泄露，比如说套接字、文件描述符、管道等也需要及时使用对应的函数释放掉。
		内存泄露是很难完全避免的，即使我们有良好的编码规范，每次通过new或者malloc申请的内存，都记得去释放，也有可能由于程序运行遇到异常导致释放的代码没能执行，导致内存泄露。
        举例：int* p = new int;  fun();  delete p; 如果执行fun()函数时遇到异常，就会导致p所指向的内存无法释放，造成内存泄露。

	2.智能指针
		为了解决内存泄露的问题，C++11引入了智能指针，它可以做到帮我们自动释放内存和操作系统的资源
		智能指针原理：
			它是采用RAII（资源获取即初始化）的技术来实现，它是一种利用对象的生命周期来控制资源（内存、、文件句柄、套接字、管道、网络连接等）的技术。
			具体做法就是在对象创建的时候获取资源，在对象生存期内保持资源的有效使用，在对象析构的时候来释放资源。把资源管理的责任交给了对象。
			实现的时候，就是创建一个类，然后给这个类添加资源获取和释放的能力，同时封装一个原始指针（裸指针）用来管理资源，同时也要具有像指针一样的行为。
			那就要重载指针的运算符，比如* 比如->
			为了让智能指针有通用性，需要将它声明为模板类，模板参数是原始指针类型。

	3.智能指针的分类
		使用智能指针需要引入头文件#include(memory>
		分类的依据是根据对内存资源的所有权，分为三类:
		shared_ptr：共享智能指针
		unque_ptr：独占智能指针
		weak_ptr：弱引用(共享)智能指针

		shared_ptr共享智能指针:
			它运行通过拷贝构造函数或者赋值函数的方式，实现多个共享智能指针共同管理一块内存，它们会共享一个引用计数，引用技术标记了当前有多少个智能指针在管理同一块内存，
			每增加一个共享指针管理者，计数会+1，每减少一个共享指针管理者，计数会-1,当引用计数为0时，才会自动释放内存。
			这样的机制可以保证这块内存同时被多个共享指针管理，而且只能被释放一次。

			共享智能指针初始化的方式：
				构造函数：shared_ptr<T> sp(要管理的内存); 如：shared_ptr<int> sp1(new int(100));
				拷贝构造：shared_ptr<T> sp(另一个已存在的共享智能指针); //此时这两个智能指针就会共同管理一块内存
				辅助函数：make_shared：如：shared_ptr<int> sp1 = make_shared<int>(100);  辅助函数效率更高
			共享智能指针的成员函数：
				get()：返回智能指针中封装的裸指针
				use_count()：返回引用计数
				reset()：不带参数的情况下，如果当前智能指针是唯一管理者，则释放内存并且置空该指针。如果不是唯一管理者，只减少引用计数，然后置空该指针
				reset(要管理的新内存地址)：带参数的情况下，如果当前智能指针是唯一管理者，则释放内存并且将指针指向新的内存地址，如果不是唯一管理者，只减少引用计数，然后将指针指向新的内存地址
				operator=()：对赋值运算符的重载，此时这两个智能指针就会共同管理一块内存

			手动指定删除动作，也叫做删除器
			在初始化智能指针的时候可以指定自己的删除函数，从而实现自己的逻辑
			手动删除其的主要作用：用来释放自定义类型的多个连续内存空间，因为此时默认的删除函数无法做到完全释放。对于基本类型的连续空间，默认删除函数是没问题的
			
			共享智能指针使用注意事项：
			1）不能直接new一个空间给智能指针，而是要使用构造函数或者辅助来管理空间
			2）可以通过一个普通指针来初始化智能指针，但是这样空间就交给智能指针管理了，就不能在通过普通指针释放空间了
			3）不用用一个普通指针（裸指针）初始化多个智能指针，造成了多次释放
			4）不用在函数的参数中来创建智能指针，而应该提前将智能指针创建好，再传入函数的参数，因为不同的编译器对函数参数的解释顺序不同。
			5）禁止在类的成员函数返回管理this指针的智能指针。
			6）避免两个类循环引用，即两个类互相含有管理对方类对象的智能指针，并且它们相互进行了引用。这会造成内存泄露。
			7）智能指针放入容器中的时候，要注意容器会单独拷贝一份，这会导致引用计数+1，所以如果想立即释放的话，需要将容器中的智能指针清空。

		
		unique_ptr独占智能指针:
			独占智能指针不允许其他的智能指针跟它共享内存，所以它也不支持赋值，只能初始化构造，不支持拷贝构造和赋值函数。
			独占智能指针的成员函数：
				构造函数：unique_ptr<T> up(要管理的内存地址);
				辅助函数：make_unique<T>;  举例：unique_ptr<int> up1 = make_unique<int>(100);
				reset()：不带参数的情况下，释放内存并且置空该指针。
				reset(要管理的新内存地址)：带参数的情况下，释放内存并且将指针指向新的内存地址。
				get()：返回智能指针中封装的裸指针
		
		
		weak_ptr弱引用(共享)智能指针:
			弱共享可以当作shared ptr的助手，它不能管理shared_ptr管理的内存，只起到旁观者的作用，使用弱共享智能指针，不会引起计数的变化。
			同时，弱共享指针也没用重载指针的运算符:*和->，所以它不能操作任何资源，只能获取资源的状态。
			弱共享智能指针的成员函数：
				构造函数：weak_ptr<T> wp(要监视的智能指针对象);
				拷贝构造：weak_ptr<T> wp(另一个已存在的弱共享智能指针);  从而实现共同监视一块内存
				operator=()：赋值函数重载，实现弱指针之间的赋值，从而实现共同监视一块内存
				use_count()：获取它所监视内存的引用计数，代表着有几个共享指针在管理
				expired()：判断是否已经被释放，失效则返回true，否则返回false
				lock()：获取所监视内存的某个智能指针对象
				reset()：重置，不再监视任何资源
			
		
		
		智能指针总结：
			1）共享智能指针通常使用在权限不明的情况下，运行多个智能指针共同管理一块内存，可以避免内存泄露。
			2）独占智能指针通常用于独占一块内存，不允许其他智能指针共享，可以避免资源竞争。不支持拷贝构造和赋值
			3）如果决定要使用智能指针帮我们管理内存的话，那就保持一致都是用智能指针，尽量减少和裸指针一起使用，牢记智能指针使用的注意事项（7条）
			4）除了内存要管理，其他的一些系统资源也需要管理，这个一般是通过自己实现的删除器来释放这些资源。
			5）使用智能指针的时候，最好提前初始化，给每个智能指针一个名字，不要在某个表达式内或者参数内初始化。
			6）对于智能指针的使用，不适合对性能要求高的程序，因为智能指针会导致性能的损耗
			7）还有一种情况，也适合使用智能指针，某个内存不确定由谁来释放的时候。


				
*/