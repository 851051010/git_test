#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>
#include <vector>
#include <condition_variable>

using namespace std;

//给线程绑定的函数
void fun1(int n)
{
	for (int i = 0; i < 5; i++)
	{
		cout << "fun1,参数：" << n << endl;
		this_thread::sleep_for(chrono::milliseconds(500)); //休眠500毫秒
	}
}
void fun2(int& n) //引用传递
{
	for (int i = 0; i < 5; i++)
	{
		cout << "fun2,参数：" << n << endl;
		n++; //引用传递，修改原变量的值
		this_thread::sleep_for(chrono::milliseconds(500)); //休眠500毫秒
	}
}
void test01()
{
	//构造线程并且绑定函数去执行
	int n = 0;
	thread t1;//无参构造函数，没有绑定函数，所以不会启动线程
	cout << "t1.get_id():" << t1.get_id() << endl; //获取线程的ID，如果没有启动线程，则返回0
	thread t2(fun1, n); //绑定fun1函数，参数为n，t2线程会马上执行
	cout << "t2.get_id():" << t2.get_id() << endl; //获取线程的ID
	thread t3(fun2, ref(n)); //绑定fun2函数，参数为n的引用，t3线程会马上执行
	cout << "t3.get_id():" << t3.get_id() << endl; //获取线程的ID
	thread t4(move(t3));//移动构造，通过move转成右值之后实现移动，t3的所有权转移给t4，t3就被析构了
	cout << "t3.get_id():" << t3.get_id() << endl; //t3的id变为0
	cout << "t4.get_id():" << t4.get_id() << endl; //获取线程的ID，此时t4的id就是原来t3的id
	//为了保证子线程可以顺利执行完毕需要进程阻塞执行或者分类执行
	t2.join(); //阻塞执行，等待t2线程执行完毕
	t4.join(); //阻塞执行，等待t4线程执行完毕
	cout << "t2和t4线程执行完毕后，n=" << n << endl;
	cout << "t2.get_id():" << t2.get_id() << endl; //t2的id变为0
	cout << "t4.get_id():" << t4.get_id() << endl; //t4的id变为0
}

void fun3()
{
	int i = 1;
	cout << "子线程打印的：" << endl;
	while (i <= 10)
	{
		cout << i << " ";
		i++;
	}
	cout << endl;
}
void test02()
{
	thread t5(fun3); //t5线程不会执行，等待我们手动执行
	t5.join(); //阻塞执行
	cout << "主线程继续打印：" << endl;
	for (int i = 11; i <= 15; i++)
	{
		cout << i << " ";
	}
	cout << endl;
}

void test03()
{
	thread t6(fun3); 
	t6.detach(); //分离到后台执行，守护执行
	cout << "主线程打印：" << endl;
	for (int i = 11; i <= 15; i++)
	{
		cout << i << " ";
	}
	cout << endl;
	//主线程不会等待子线程
}

void fun4() { cout << "执行fun4" << endl; }
void test04()
{
	thread t7(fun4); //t7线程不会执行
	cout << "t7 joinable():" << t7.joinable() << endl; //t7的状态是joinable的,1
	t7.detach(); 
	cout << "t7 joinable():" << t7.joinable() << endl; //t7的状态是detach的,0
}


//3.线程互斥
int num = 0; //普通整型，需要加锁才能保证数据的安全
atomic_int sum = 0; //原子整型，它不用加锁也可以保证数据的安全
mutex num_lock; //互斥锁，用来控制对num的访问
void fun5() //没有加锁，这个函数在多线程下是不安全的
{
	for (int i = 0; i < 5; i++)
	{
		cout << this_thread::get_id() << ":" << ++num << endl;
	}
}
void test05()
{
	thread t8(fun5);
	thread t9(fun5);
	t8.join();
	t9.join();
}

void fun6() //加锁，这个函数在多线程下是安全的
{
	num_lock.lock(); //申请锁,后面的代码就是独占资源了
	for (int i = 0; i < 5; i++)
	{
		cout << this_thread::get_id() << ":" << ++num << endl;
	}
	num_lock.unlock(); //释放锁
}
void test06()
{
	thread t8(fun6);
	thread t9(fun6);
	thread t10(fun6);
	t8.join();
	t9.join();
	t10.join();
}

void fun7() //不需要加锁也能保证安全性，因为使用了原子整型
{
	for (int i = 0; i < 5; i++)
	{
		cout << this_thread::get_id() << ":" << ++sum << endl;
	}
}
void test07()
{
	thread t8(fun7);
	thread t9(fun7);
	thread t10(fun7);
	t8.join();
	t9.join();
	t10.join();
}
//大量操作，尝试一下，看看int类型会不会出错
void fun_int() //不加锁的情况下，操作int类型，执行10000次
{
	for (int i = 0; i < 10000; i++)
	{
		num++;
	}
}
void test08()
{
	//多启动一些线程，启动100个线程，每个线程执行10000次int类型的++操作,所以num最终的结果为1000000
	vector<thread> vec;
	for (int i = 0; i < 100; i++)
	{
		vec.push_back(thread(fun_int));
	}
	for (int i = 0; i < 100; i++)
	{
		vec[i].join();
	}
	cout << "num = " << num << endl;  //如果不是1000000，说明出错了
}

void fun_atomic() //使用原子整型，不用加锁，也能保证安全性
{
	for (int i = 0; i < 10000; i++)
	{
		sum++;
	}
}
void test09()
{
	//多启动一些线程，启动100个线程，每个线程执行10000次int类型的++操作,所以num最终的结果为1000000
	vector<thread> vec;
	for (int i = 0; i < 100; i++)
	{
		vec.push_back(thread(fun_atomic));
	}
	for (int i = 0; i < 100; i++)
	{
		vec[i].join();
	}
	cout << "sum = " << sum << endl;  //1000000，使用原子整型，不会出错
}


//线程同步，生产者消费者模型
mutex g_mtx; //互斥锁
condition_variable g_convar; //条件变量
vector<int> g_vector; //存放产品的容器，它是生产者和消费者的共享资源
//生产者线程绑定的函数
void producter()
{
	//模拟10次产品生产，每生产一个，就通知消费者线程一次
	for (int i = 0; i < 10; i++)
	{
		//生产的过程就是操作容器的过程，容器是共享，所以需要加锁保证安全
		//加锁，不是使用原始的mutex锁实现的，而是使用unique_lock<T>类来实现，这个类模板其实就是对mutex原始锁进一步的封装，从而提供了更多的方法
		//它的技术类似于智能指针技术，IRAII技术，可以帮我们自动实现锁的获取和释放。
		unique_lock<mutex> lock(g_mtx); //申请锁

		//接下来写生产的逻辑
		//作为生产者，需要判断容器中还有没有产品，如果有产品就不需要生产，只需要等待消费者去消费
		while (!g_vector.empty()) //容器不为空，说明还有产品，生产者等待消费者消费,不需要生产
		{
			g_convar.wait(lock); //等待条件变量通知，如果条件变量通知了，就继续执行，否则就阻塞在这里
			//wait函数可以做三件事情：1.释放锁，2.阻塞线程，直到消费者发来生产的通知，3.重新申请锁，进行生产。
		}
		//跳出循环后，代表容器中没有产品，可以生产了
		g_vector.push_back(i); //生产产品
		cout << "生产者生产了：" << i << endl;
		//通知消费者线程，可以消费了
		g_convar.notify_all(); //通知消费可以消费了，唤醒所有等待的消费者线程
		this_thread::sleep_for(chrono::milliseconds(500)); //休眠500毫秒
	}	
}
//消费者线程绑定的函数
void consumer()
{
	for (int i = 0; i < 10; i++)
	{
		unique_lock<mutex> lock(g_mtx); //申请锁
		while (g_vector.empty()) //容器为空，说明没有产品，消费者等待生产者生产,不需要消费
		{
			g_convar.wait(lock); //等待条件变量通知，如果条件变量通知了，就继续执行，否则就阻塞在这里
			//wait函数可以做三件事情：1.释放锁，2.阻塞线程，直到生产者发来消费的通知，3.重新申请锁，进行消费。
		}
		//跳出循环后，代表容器中有产品，可以消费了
		cout << "消费者消费了产品：" << g_vector.back() << endl;
		g_vector.pop_back();//消费完了产品，从容器中删除
		g_convar.notify_all(); //通知生产可以生产了，唤醒所有等待的生产者线程
		this_thread::sleep_for(chrono::milliseconds(500)); //休眠500毫秒
	}
}
void test10()
{
	thread t_p(producter); //生产者线程
	thread t_c(consumer); //消费者线程
	t_p.join(); //等待生产者线程执行完毕
	t_c.join(); //等待消费者线程执行完毕
}



int main()
{
	//test01();
	//cout << endl;
	//test02();
	//cout << endl;
	//test03();
	//cout << endl;
	//test04();
	//cout << endl;
	//test05();
	//cout << endl;
	//test06();
	//cout << endl;
	test07();
	cout << endl;
	//test08();
	//cout << endl;
	//test09();
	//cout << endl;
	//test10();
	//cout << endl;
	return 0;
}


/*
多线程编程
	1.多线程概念
		程序：程序就是代码和数据的组合，程序不执行的时候，跟文件没什么区别，是一个静态的东西。
		进程：程序的执行过程，当程序运行起来，就成为了进程。此时是动态的。
		线程：线程是进程中实际运行的单位，也是操作系统调度的最小单位。所以每个进程至少包括一个线程。如果启用了多个线程，那就是多线程编程。
	
	2.thread基本应用
		在C++11之前，C++库没有提供多线程的接口，这个时候如果要做多线程编程，只能使用操作系统的多线程接口。这样做最大的问题是代码的复杂性以及无法跨平台。
		在C++11之后，C++标准库提供了多线程的接口，可以方便地编写多线程程序。而且可以跨平台。这个库叫做thread，要引入这个库。
		thread的构造函数：
			1）thread();默认构造函数，创建一个空的线程对象。它没有关联绑定任何可执行的函数，所以也不会启动真正的线程。
			2）thread t(func); 这个线程绑定了一个函数func，但它不会马上执行，而是等待我们给它选择之心那个的方式：阻塞执行（join）、分离/守护执行（detach）
				阻塞执行：使用join函数，阻塞的意思就是在某处停下，这时候要等待当前阻塞的这个线程执行完毕，然后才能继续往后执行。
					假如说我们在main函数中启动了一个线程t，这个时候在t线程之上还有一个父线程main函数的线程，
					main函数的线程对t线程是有管理权限的，也就是说父线程可以管理子线程。如果父线程执行结束了，它会杀掉所有没有执行结束的子线程。
					这样就会导致子线程的任务无法完成。怎么保证子线程的任务能全部完成呢?那就是通过join函数指定子线程采用阻塞的方式去执行，
					这样父线程就会等待子线程执行完毕，然后再继续往下执行。
				分离/守护执行：使用detach函数,这个时候当前线程是跟主线程分开的，互不影响，各自执行各自的。也可以成为守护线程，子线程不受主线程管控，注意：分离的线程不能再join，只能detach。
			3）thread t(func, args1, args2, ...); 这个线程绑定了func函数，同时给func函数传递了参数args1,args2...这种情况下，线程不需要我们手动执行，它会马上开始执行，
			   但是为了保证线程任务能顺利完成，我们仍然需要指定阻塞或者分类的方式。
			4）拷贝构造是被禁止的，因为线程对象是不可拷贝的，所以我们不能将一个线程对象赋值给另一个线程对象。
			5）移动构造：thread t(thread&& other); 直接通过移动构造取得线程的所有权。
		thread的常用函数：
			1）get_id()：获取线程的ID，这个ID是系统分配的，是一个整型值。
			2）joinable()：返回bool，判断线程的一种状态。如果一个线程已经执行了join或者detach,那么它的状态就不再是joinable了，此时返回false
			   当thread对象析构的时候，会判断joinable的状态，如果当前对象是joinable为true的，就会调用一个系统函数结束掉这个线程，这样会导致异常
			3）join()：阻塞执行
			4）detach()：分离/守护执行
		休眠函数：
			不同的操作系统本身都有自己的休眠函数接口
			C++11之后，C++也提供了专门的休眠函数，这样就可以跨平台了。添加头文件#includ <thread>，然后就可以使用了。
			this_thread::sleep_for(休眠时间); // 休眠1秒
			如：this_thread::sleep_for(chrono::milliseconds(500)); // 休眠500毫秒


	3.线程互斥
		在多线程环境下运行的代码，需要考虑更多问题。主要考虑的就是线程的互斥问题，因为在多线程环境下，有些代码的运行是不可靠的不安全的，这些代码
		我们称之为临界区代码，如果不做处理的话，不能直接运行在多线程环境下。这种临界区代码我们需要在运行之前，保证对资源的独占性，也就是说要保证
		只有某一个线程在独占资源的情况下执行这些代码，其他线程不能干预。
			线程互斥的实现方式：
				互斥锁：互斥锁是一种同步机制，用来控制对共享资源的访问，防止多个线程同时访问共享资源。互斥锁的实现方式是，在访问共享资源之前，先申请锁，
				        如果锁已经被其他线程占用，那么申请锁的线程就会被阻塞，直到锁被释放。需要添加头文件#include <mutex>，通过lock和解锁unlock函数实现锁定。
				除此之外，thread类库还提供了更加轻量级的原子操作类，它是基于CAS技术实现的。CAS技术的原理就是在每次对资源修改之前，都要对资源现在的值
				跟预期的正确值进行比对，如果对的上才去修改资源，对不上就不能修改，保证资源的正确性。需要添加头文件#include <atomic>		


	4.线程同步
		线程在运行中是通过操作系统的调度算法进行调度的，每个线程占用CPU的时间片执行指令，所以在调度的过程中，每个线程没有固定的顺序可言。
		但是有些情况下，我们确实需要线程和线程之间进行一些同步的通信机制，让线程之间有配合。这就是线程同步。比如说某个线程需要等待另外一个线程的执行结果，
		才能继续执行，这就是线程同步的一种。
			线程同步最经典的例子就是生产者-消费者模型。生产者消费者两个线程，它们要相互配合。生产者负责生成产品，它生成出来产品后，就要通知
			消费者线程去消费产品：消费者消费完了产品之后，就没有产品了，这时候消费者再去通知生产者线程去生产产品。
			线程同步使用的是条件变量来传递信息的，条件变量可以解决线程之间通信的问题。需要引入头文件 #include <condition_variable>
			同步的前提是互斥，所以条件变量要和互斥锁一起使用。


	总结：
		首先，多线程编程比较复杂，如果你的思维没有调整过来，就尽量不要用，因为用不好容器出问题，而且适合多线程的场景并没有那么多。
		多线程编程也不一定会比单线程效率高。
		只有在适合多线程的需求下，才建议使用多线程编程，比如：
			1.web服务器：多线程可以处理大量用户的连接请求，提高服务器的响应能力
			2.涉及到大量复杂运算的任务，比如说AI算法，比如图像处理，比如自然语言处理，比如智能驾驶，比如大模型等等，适合使用多线程来处理不同的计算任务
			3.游戏开发：适合使用多线程
			4.爬虫：适合使用多线程，可以提高爬虫的效率
			5.工业领域：多线程可以提高处理能力，比如：机器人控制，工业控制，自动化控制等等。


*/