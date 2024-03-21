
struct MemoryPool<T> {
    ele: T,
    next: Option<Box<MemoryPool<T>>,
}

struct MemoryPoolHashMap<T>{
    hMap: HashMap<i32, Option<Box<MemoryPool<T>>>,
}

pub trait poolAlloc<T> {

    /// Alloc resource
    pub fn alloc(&mut self, &mut ) -> T;

    /// Add resource
    pub fn add(&mut self, src: T);

    /// Recycle resource
    pub fn recycle(&mut self, src: T);

    /// Remove resource
    pub fn remove(&mut self);
}

impl linked<T> for MemoryAllocator<T> {

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
        self.get_last_mut().next = Some(Box::new(_self));
    }

    pub fn remove_head(&mut self, val: T) -> T {
        if let Some(_self) = self.next {
            let res = _self.ele;
            self.next = Some(_self.next.take());
            res
        }else{
            panic!("Remove the empty linked list.");
        }
    }
}

impl<T> poolAlloc<T> for MemoryPoolHashMap<T> {

    pub fn newPool() -> Self {
        Self {
            hMap: HashMap::new(),
        }
    }

    pub fn alloc(&mut self,memAlloc: &mut MemoryAllocator<T>,id: i32) -> T {
        
    }
}