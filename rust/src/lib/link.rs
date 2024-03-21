use std::rc::Rc;
use std::cell::RefCell;

pub trait linked<T> {

    /// Create a new Node.
    pub fn new(val: T) -> T {
        Node{val: val, next: None};
    }

    /// Return the last node borrow mut.
    pub fn get_last_mut(&mut self) -> &mut self {
        if let Some(ref mut boxNode) = self.next {
            boxNode.get_last_mut()
        }
        else{
            self
        }
    }

    /// Add node.
    pub fn append(&mut self, val: T) {
        let _node = Node::new(val);
        self.get_last_mut().next = Some(Box::new(_node));
    }

    /// Remove the head node.
    pub fn remove_head(&mut self, val: T){
        if is_some(self.next)
            self.next = self.next.take();
        else
            panic!("Remove the empty linked list.");
    }
}

