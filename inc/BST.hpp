#pragma once

#include <list>
#include <cassert>
#include <stack>
#include <ostream>

template <typename T>
class BST final 
{
    using height_t = uint8_t;
    using bfactor_t = int;

    class BinNode final
    {
    private:
        const T val_;
        BinNode *left_;
        BinNode *right_;
        height_t height_;
    public:
        BinNode(T val, BinNode* left = nullptr, BinNode* right = nullptr) 
            : val_(val), left_(nullptr), right_(nullptr), height_(1) {};

        const T& val() const {return val_;}
        BinNode *get_left()  const {return left_;}
        BinNode *get_right() const {return right_;}
        void set_left (BinNode *new_left)  {left_  = new_left;}
        void set_right(BinNode *new_right) {right_ = new_right;}
        
        bfactor_t bfactor() const {return height_smart(right_) - height_smart(left_);}
        static height_t height_smart(BinNode *node_ptr) {return (node_ptr ? node_ptr->height_ : 0);} 
        height_t update_height(); // assumes left and right heights are updated
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

    enum dir_t {LEFT, RIGHT};

private:
    BinNode *root_;
    NodeOwner node_owner_;

    BinNode *copy_subtree(const BinNode &ref_subtree_root);
    BinNode *balance_node(BinNode *node);
    BSTIterator upper_lower_bound_helper(const T& val, dir_t search_dir);
    void dump_subtree(std::ostream &stream, BinNode *subtree_root);
public:
    BST() : root_(nullptr) {};
    ~BST() = default;
    BST(const BST& rhs);
    BST(BST&& rhs) = default; //REVIEW - will std::move(node_owner_) and root_ = rhs.root_ ?
    BST& operator= (const BST& rhs);
    // BST& operator= (BST&& rhs); // TODO

    BSTIterator insert(T val);
    BSTIterator lower_bound(const T& val) const;
    BSTIterator upper_bound(const T& val) const;

    BSTIterator begin() const;
    BSTIterator end() const;

    void swap(const BST& rhs) noexcept;

    void dump(std::ostream &stream);
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
typename BST<T>::BSTIterator BST<T>::insert(T val)
{
    if (!root_) 
    {
        root_ = node_owner_.create({val});
        return BSTIterator{root_};
    }

    std::stack<std::pair<BinNode*, dir_t>> path;
    BinNode *curr_node = root_;
    while (curr_node)
    {
        if (val < curr_node->val())
        {
            path.push({curr_node, dir_t::LEFT});
            curr_node = curr_node->get_left();
        }
        else if (val > curr_node->val())
        {
            path.push({curr_node, dir_t::RIGHT});
            curr_node = curr_node->get_right();
        }
        else return BSTIterator{curr_node}; // elem already exists
    }

    BinNode *created_node = node_owner_.create({val});
    auto [last_node, last_dir] = path.top(); path.pop();
    if      (last_dir == dir_t::LEFT)  last_node->set_left(created_node);
    else if (last_dir == dir_t::RIGHT) last_node->set_right(created_node);
    last_node->update_height();
    
    while (!path.empty())
    {
        auto [node, dir] = path.top(); path.pop();

        node->update_height();
        if      (dir == dir_t::LEFT)  node->set_left(balance_node(node->get_left()));
        else if (dir == dir_t::RIGHT) node->set_right(balance_node(node->get_right()));
    }

    root_ = balance_node(root_);

    return BSTIterator{created_node};
}


template <typename T>
typename BST<T>::BSTIterator BST<T>::upper_lower_bound_helper(const T &val, dir_t search_dir)
{
    //if (!root_) return ... // TODO iterator end()

    std::stack<std::pair<BinNode*, dir_t>> path;
    BinNode *curr_node = root_;
    while (curr_node)
    {
        if (val < curr_node->val())
        {
            BinNode *curr_node_left = curr_node->get_left(); 
            if (curr_node_left) 
            {
                path.push({curr_node, dir_t::LEFT});
                curr_node = curr_node_left;
            }
            else
            {
                if (search_dir == dir_t::LEFT) return BSTIterator{curr_node};
                else break;
            } 
        }
        else if (val > curr_node->val())
        {
            BinNode *curr_node_right = curr_node->get_right();
            if (curr_node_right) 
            {
                path.push({curr_node, dir_t::RIGHT});
                curr_node = curr_node_right;
            }
            else
            {
                if (search_dir == dir_t::RIGHT) return BSTIterator{curr_node};
                else break;
            }
        }
        else return BSTIterator{curr_node}; // elem itself
    }    

    while(!path.empty())
    {
        auto [node, dir] = path.top(); path.pop();
        if (dir == search_dir)
            return BSTIterator{node};
    }
    
    assert(0);
    return BSTIterator{nullptr};
}

template <typename T>
typename BST<T>::BSTIterator BST<T>::lower_bound(const T& val) const
{
    return upper_lower_bound_helper(val, dir_t::LEFT);
}

template <typename T>
typename BST<T>::BSTIterator BST<T>::upper_bound(const T &val) const
{
    return upper_lower_bound_helper(val, dir_t::RIGHT);
}

template <typename T>
void BST<T>::swap(const BST &rhs) noexcept
{
    std::swap(node_owner_, rhs.node_owner_);
    std::swap(root_, rhs.root_);
}

template <typename T>
void BST<T>::dump_subtree(std::ostream &stream, BinNode *subtree_root)
{
    assert(subtree_root);

    stream << "( ";
    if (subtree_root->get_left()) 
        dump_subtree(stream, subtree_root->get_left());
    else
        stream << "nil";
    stream << " {" << subtree_root->val() << "} ";
    if (subtree_root->get_right())
        dump_subtree(stream, subtree_root->get_right());
    else
        stream << "nil";
    stream << " ) ";
}

template <typename T>
void BST<T>::dump(std::ostream &stream)
{
    if (root_) dump_subtree(stream, root_);
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

template <typename T>
typename BST<T>::height_t BST<T>::BinNode::update_height()
{
    height_t hl = height_smart(left_), hr = height_smart(right_);
    return (height_ = (hl > hr ? hl : hr) + 1);
}

template <typename T>
typename BST<T>::BinNode *BST<T>::balance_node(BinNode *node)
{
    auto rotate_left = [](BinNode *q)
    {
        BinNode* p = q->get_right();
        q->set_right(p->get_left());
        p->set_left(q);
        q->update_height();
        p->update_height();
        return p;
    };

    auto rotate_right = [](BinNode *p)
    {
        BinNode* q = p->get_left();
        p->set_left(q->get_right());
        q->set_right(p);
        p->update_height();
        q->update_height();
        return q;
    };

    bfactor_t node_bfactor = node->bfactor();
    if(node_bfactor == 2)
	{
        BinNode *node_right = node->get_right();
		if(node_right->bfactor() < 0)
			node_right = rotate_right(node_right);
		return rotate_left(node);
	}
	if(node_bfactor == -2)
	{
        BinNode *node_left = node->get_left();
		if(node_left->bfactor() > 0)
			node_left = rotate_left(node_left);
		return rotate_right(node);
	}
	return node; // balancing isn't needed
}
