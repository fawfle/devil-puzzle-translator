template <typename T> struct DLLNode {
	public:
		DLLNode<T> *left = this;
		DLLNode<T> *right = this;
		T data;
};

template <typename T> struct DLL
{
	public:
		DLLNode<T> *head = nullptr;
		DLLNode<T> *append(T data);
		
		// add/remove existing nodes
		void insert(DLLNode<T> *node);
		void remove(DLLNode<T> *node);
};

template <typename T>
DLLNode<T> *DLL<T>::append(T data) {
	DLLNode<T>* node = new DLLNode<T>();
	node->data = data;
	
	if (head == nullptr) {
		head = new DLLNode<T>();
	}

	node->left = head->left;
	node->right = head;

	head->left->right = node;
	head->left = node;

	
	return node;
}

template <typename T>
void DLL<T>::insert(DLLNode<T> *node) {
	node->left->right = node;
	node->right->left = node;
}

template <typename T>
void DLL<T>::remove(DLLNode<T> *node) {
	node->left->right = node->right;
	node->right->left = node->left;
}
