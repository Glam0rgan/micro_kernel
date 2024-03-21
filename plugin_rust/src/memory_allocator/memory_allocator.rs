use os::link;

struct MemoryAllocator<T> {
    ele: T,
    next: Option<Box<MemoryAllocator<T>>,
}

pub trait allocator<T> {

    /// Initiatate the allocator.
    pub fn init(&mut self);

    /// Alloc resource
    pub fn alloc(&mut self) -> T;

    /// Add resource
    pub fn add(&mut self, src: T);

    /// Recycle resource
    pub fn recycle(&mut self, src: T);

    /// Remove resource
    pub fn remove(&mut self);
}

impl<T> linked<T> for MemoryAllocator<T> {

    pub fn new(val: T) -> T {
        Self{val: val, next: None};
    }

    pub fn get_last_mut(&mut self) -> &mut self {
        if let Some(ref mut boxSelf) = self.next 
            boxSelf.get_last_mut()
        else
            self
    }

    pub fn append(&mut self, val: T) {
        let _self = Self::new(val);
        self.get_last_mut().next = Some(Box::new(_node));
    }

    pub fn remove_head(&mut self, val: T) -> T {
        if let Some(_self) = self.next {
            let res = _self.ele;
            self.next = Some(_self.next.take());
            res
        }else {
            panic!("Remove the empty linked list.");
        }
    }
}

impl<T> allocator<T> for MemoryAllocator<T> {

    pub fn newAlloc() -> MemoryAllocator<T> {
        MemoryAllocator<T> {
            ele: 0,
            next: None,
        }
    }
    
    pub fn init(&mut self) {
        // Alloc 128MB memory from kernel
        let memStart : T = brk(32768);
        let mut index = 0;

        while(index <= 32768 - 1) {
            add(self, memStart + index * 4096 as T);
        }
    }

    pub fn alloc(&mut self) -> T {
        self.remove_head()
    }

    pub fn add(&mut self, src: T) {
        append(self, T);
    }
    
    pub fn recycle(&mut self, src: T) {
        append(self, T);
    }

    pub fn remove(&mut self) {
        self.remove_head();
    }
}