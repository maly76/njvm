import java.util.ArrayList;

class Test
{    
    Stack stack = new Stack();  // the values are first time all on the stack
    CircularLinkedList linkedList = new CircularLinkedList();
    Queue circle = new Queue();

    public void showOutputs()
    {
        stack.push(12); stack.push(5); stack.push(33); stack.push(18); stack.push(4);
        for (int i = 0; i < stack.size; i++)
        {
            int number = stack.pop();
            Node<Integer> node = new Node<Integer>(number);
            if (number % 3 == 0)    //check if it is divisable by 3, if yes put it in the circle
                circle.enqu(number);       
            else
                linkedList.inserFirst(node);     // if not then put it in the linked list
        }

        //   PRINT OUTPUTS
        System.out.println("\nOutput 1: \n");
        for (int i = 0; i < circle.size; i++)
        {
            if (circle.isempty() == false)
            {
                int number = circle.dequ();
                System.out.print(number + "   ");
            }
        }

        System.out.println("\nOutput 2: \n");
        for (int i = 0; i < linkedList.size; i++)
        {
            Node node = linkedList.removeFirst();
            if (node != null)
            {
                int number = (int)node.getData();
                System.out.print(number + "   ");  
            } 
        }

        System.out.println("\nOutput 3: \n");
        for (int i = 0; i <= linkedList.size; i++)
        {
            Node node = linkedList.removeFirst();
            int number = (int)node.getData();
            System.out.print(number * number + "   "); 
        }
    }
}

class Main{
    public static void main(String[] args) {
        Test test = new Test();
        test.showOutputs();
        System.out.println("\n");
    }
}

class Stack{
    int size=5 ;
    int []a = new int[size];
    int top=-1;

public  void push(int val){
   if(isfull()){
       System.out.println("Stack is Full.");
   }
   else{
       top=top+1;
       a[top]=val;
       System.out.println("The value is pushed in Stack = "+val);
   }
}
public int pop(){
    int x=0;
    if(isempty()){
           System.out.println("Empty!!");
           return 0;
   }
   else{
       x=a[top];
       top=top-1;
       System.out.println(x);
       return x;
   }
}

public  boolean isempty(){
    if(top==-1){
        return true;
    }else 
        return false;
}

public  boolean isfull(){
    if(top==size-1){
        return true;
    }else 
        return false;
}

}


class Queue{
    int size=5 ;
        int []Q = new int[size];
        int numitem=0;
        int front=0;
        int  rear=0;
   
    public  void enqu(int val){
        if(isfull()){
           System.out.println("Queue is Full.");
        }
        else{
           Q[rear]=val;
           rear=(rear+1)%size;
          numitem++;
           System.out.println("The value is pushed in Queue = "+val);
        }
    }
    public int dequ(){
          
	if(isempty()){
            System.out.println("Queue is Empty!!");
            return 00;
        }
        else{
            int x=Q[front];
            front = (front +1)%size;
            numitem--;               
            return x;
        }
    }
    
    public  boolean isempty(){
        if(numitem==0){
            return true;
        }else 
            return false;
    }
    
    public  boolean isfull(){
        if(numitem==size){
            return true;
        }else 
            return false;
    }

}



 class CircularLinkedList {

    Node head = null;
    Node tail = null;
    int size = 0;

    public void inserFirst(Node newNode) {
        if (size == 0) {
            head = newNode;
            tail = newNode;
            tail.next = newNode;
        } else {
            newNode.next = head;
            head = newNode;
            tail.next = head;
        }
        size++;
    }

    public void insertLast(Node newNode) {
        if (size == 0) {
            head = newNode;
            tail = newNode;
            tail.next = newNode;
        } else {
            newNode.next = head;
            tail.next = newNode;
            tail = newNode;
        }
        size++;
    }

    public void insertAfter(Node currentNode, Node newNode) {
        if (size == 0) {
            head = newNode;
            tail = newNode;
            tail.next = head;
            size++;
        } else {
            if (currentNode == tail) {
                insertLast(newNode);
            } else {
                newNode.next = currentNode.next;
                currentNode.next = newNode;
                size++;
            }
        }

    }

    public void dispalyList() {
        if (size == 0) {
            System.out.println("List is empty");
        } else {
            Node currentNode = head;
            for (int i = 1; i <= size; i++) {
                System.out.print(currentNode.data + " ");
                currentNode = currentNode.next;
            }
        }
    }

    public Node removeFirst() {
        if (head == null) {
            System.out.println("list is empty");
            return null;
        } else {
            Node removedNode = head;
            tail.next = head.next;
            head = head.next;
            size--;
            return removedNode;
        }
    }

}

class Node<T> {

    T data;
    Node<T> next;

    public Node(T data, Node<T> next) {
        this.data = data;
        this.next = next;
    }

    public Node(T data) { 
        this(data, null); 
    }

    public T getData() { 
        return data; 
    }

    public void setData(T data) { 
        this.data = data; 
    }

    public Node<T> getNext() { 
        return next; 
    }

    public void setNext(Node<T> next) { 
        this.next = next; 
    }
     
}
