    struct demo : koku::managed<demo> {
      int a;
      demo(int a): a(a) {}
    };
    
    #include <iostream>
    int main() {
      // auto p = new demo; //error: call to deleted function
      demo::pointer p = demo::new_instance(5);
      p->a = 10;
      std::cout << "p->a: " << p->a << std::endl;
      // demo s(2);
      // demo* a = &s; //error: candidate function has been explicitly deleted
    }
