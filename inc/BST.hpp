#pragma once

#include <list>

template <typename T>
class BST final 
{
    class BinNode final
    {
    private:
        const T val_;
        BinNode *left_;
        BinNode *right_;
    public:
        BinNode(T val, BinNode* left = nullptr, BinNode* right = nullptr) 
            : val_(val), left_(nullptr), right_(nullptr) {};
    };

    class NodeOwner final
    {
    private:
        std::list<BinNode*> own_nodes_;
    public:
        NodeOwner() {};
        NodeOwner(NodeOwner&&) = default; // REVIEW - will std::move(own_nodes_) ?
        ~NodeOwner();

        NodeOwner(const NodeOwner&)             = delete;
        NodeOwner& operator= (const NodeOwner&) = delete;
        NodeOwner& operator= (NodeOwner&&)      = delete;

        BinNode *create(const BinNode& node);
        void delete_all();
    };

    class BSTIterator final
    {
    private:
        BinNode *ptr_;
    public:
        BSTIterator(BinNode *ptr) : ptr_(ptr) {};
        
        // TODO operator++, operator*, operator->
    };

private:
    BinNode *root_;
    NodeOwner node_owner_;

    BinNode *copy_subtree(const BinNode &ref_subtree_root);
public:
    BST() : root_(nullptr) {};
    ~BST() = default;
    BST(const BST& rhs);
    BST(BST&& rhs) = default; //REVIEW - will std::move(node_owner_) and root_ = rhs.root_ ?
    BST& operator= (const BST& rhs);
    // BST& operator= (BST&& rhs); // TODO

    void swap(const BST& rhs) noexcept;
};

template <typename T>
typename BST<T>::BinNode *BST<T>::copy_subtree(const BinNode &ref_subtree_root)
{
    BinNode *ref_left  = ref_subtree_root.left_, *ref_right = ref_subtree_root.right_;
    BinNode *left = nullptr, *right = nullptr, *res = nullptr;
    if (ref_left)
        left = copy_subtree(ref_left);
    if (ref_right)
        right = copy_subtree(ref_right);  

    return node_owner_.create(BinNode{ref_subtree_root.val_, left, right});
}

template <typename T>
BST<T>::BST(const BST &rhs)
{
    BinNode *node_root = nullptr;
    try
    {
        node_root = copy_subtree(rhs.root_);    
    }
    catch(...)
    {
        node_owner_.delete_all();
        throw;
    }
    root_ = node_root;
}

template <typename T>
BST<T> &BST<T>::operator=(const BST &rhs)
{
    BST<T> tmp{rhs};
    
    swap(tmp);
    return *this;
}

template <typename T>
void BST<T>::swap(const BST &rhs) noexcept
{
    std::swap(node_owner_, rhs.node_owner_);
    std::swap(root_, rhs.root_);
}

template <typename T>
BST<T>::NodeOwner::~NodeOwner()
{
    delete_all();
}

template <typename T>
typename BST<T>::BinNode *BST<T>::NodeOwner::create(const BinNode &node)
{
    BinNode *ptr = nullptr;
    try
    {
        ptr = new BinNode{node};
        own_nodes_.push_back(ptr);
    }
    catch (...)
    {
        delete ptr;
        throw;
    }
    return ptr;
}

template <typename T>
void BST<T>::NodeOwner::delete_all()
{
    for (auto elem : own_nodes_)
        delete elem;
}
