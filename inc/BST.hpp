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
        BinNode *parent_;
        height_t height_;
    public:
        BinNode(T val, BinNode* left = nullptr, BinNode* right = nullptr) 
            : val_(val), left_(nullptr), right_(nullptr), parent_(nullptr), height_(1) {};

        const T& val() const {return val_;}
        BinNode *get_left()  const {return left_;}
        BinNode *get_right() const {return right_;}
        void set_left (BinNode *new_left);
        void set_right(BinNode *new_right);
        BinNode *get_parent() const {return parent_;}
        bool is_right_child() const {if (parent_) return (parent_->right_ == this); else return false;}
        bool is_left_child()  const {if (parent_) return (parent_->left_  == this); else return false;} 
        
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
        const BST<T> &bst_ptr_;
        BinNode *ptr_;
    public:
        BSTIterator(const BST<T> &bst_ptr, BinNode *ptr) : bst_ptr_(bst_ptr), ptr_(ptr) {};

        // UB if end() is dereferenced
        const T& operator*() const {return ptr_->val();}
        const T* operator->() const {return &(ptr_->val());}

        bool operator==(const BSTIterator& rhs) const {return ptr_ == rhs.ptr_;}
        bool operator!=(const BSTIterator& rhs) const {return !operator==(rhs);}

        BSTIterator& operator++();
        BSTIterator  operator++(int);
        BSTIterator& operator--();
        BSTIterator  operator--(int);

    };

    enum dir_t {LEFT, RIGHT};

private:
    BinNode *root_;
    NodeOwner node_owner_;
    BinNode *min_val_node_;
    BinNode *max_val_node_;

    BinNode *copy_subtree(const BinNode &ref_subtree_root);
    BinNode *balance_node(BinNode *node);
    BSTIterator upper_lower_bound_helper(const T& val, dir_t search_dir) const;
    void dump_subtree(std::ostream &stream, BinNode *subtree_root);
public:
    BST() : root_(nullptr), min_val_node_(nullptr), max_val_node_(nullptr) {};
    ~BST() = default;
    BST(const BST& rhs);
    BST(BST&& rhs) = default; //REVIEW - will std::move(node_owner_) and root_ = rhs.root_ ?
    BST& operator= (const BST& rhs);
    // BST& operator= (BST&& rhs); // TODO

    BSTIterator insert(T val);
    BSTIterator lower_bound(const T& val) const;
    BSTIterator upper_bound(const T& val) const;

    BSTIterator begin() const {return BSTIterator{*this, min_val_node_};}
    BSTIterator end() const {return BSTIterator{*this, nullptr};}

    // BSTIterator min_iter() const {return BSTIterator{*this, min_val_node_};}
    // BSTIterator max_iter() const {return BSTIterator{*this, max_val_node_};}

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
        min_val_node_ = root_;
        max_val_node_ = root_;
        return BSTIterator{*this, root_};
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
        else return BSTIterator{*this, curr_node}; // elem already exists
    }

    BinNode *created_node = node_owner_.create({val});
    auto [last_node, last_dir] = path.top(); path.pop();
    if      (last_dir == dir_t::LEFT)  last_node->set_left(created_node);
    else if (last_dir == dir_t::RIGHT) last_node->set_right(created_node);
    last_node->update_height();

    if (val < min_val_node_->val()) min_val_node_ = created_node;
    if (val > max_val_node_->val()) max_val_node_ = created_node;
    
    while (!path.empty())
    {
        auto [node, dir] = path.top(); path.pop();

        node->update_height();
        if      (dir == dir_t::LEFT)  node->set_left(balance_node(node->get_left()));
        else if (dir == dir_t::RIGHT) node->set_right(balance_node(node->get_right()));
    }

    root_ = balance_node(root_);

    return BSTIterator{*this, created_node};
}

template <typename T>
typename BST<T>::BSTIterator BST<T>::lower_bound(const T& val) const
{
    if (!root_) return end();

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
                return BSTIterator{*this, curr_node};
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
                break;
        }
        else return BSTIterator{*this, curr_node}; // elem itself
    }    

    while(!path.empty())
    {
        auto [node, dir] = path.top(); path.pop();
        if (dir == dir_t::LEFT)
            return BSTIterator{*this, node};
    }
    
    return end();
}

template <typename T>
typename BST<T>::BSTIterator BST<T>::upper_bound(const T &val) const
{
    auto lower_bound_it = lower_bound(val);
    if (lower_bound_it == end())
        return end();

    if (*lower_bound_it == val)
        return ++lower_bound_it;
    else
        return lower_bound_it;
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
void BST<T>::BinNode::set_left(BinNode *new_left)
{
    left_ = new_left; 
    if (new_left) 
        new_left->parent_ = this;
}

template <typename T>
void BST<T>::BinNode::set_right(BinNode *new_right)
{
    right_ = new_right;
    if (new_right)
        new_right->parent_ = this;
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
			node->set_right(rotate_right(node_right));
		return rotate_left(node);
	}
	if(node_bfactor == -2)
	{
        BinNode *node_left = node->get_left();
		if(node_left->bfactor() > 0)
			node->set_left(rotate_left(node_left));
		return rotate_right(node);
	}
	return node; // balancing isn't needed
}

template <typename T>
typename BST<T>::BSTIterator& BST<T>::BSTIterator::operator++()
{
    if (!ptr_) return *this; // we are end(), nowhere to increment

    if (ptr_ == bst_ptr_.max_val_node_)
    {
        ptr_ = nullptr; //becoming end()
        return *this;
    }

    if (ptr_->get_right())
    {
        ptr_ = ptr_->get_right();
        while (ptr_->get_left()) ptr_ = ptr_->get_left();
        return *this;
    }

    BinNode *curr_node = ptr_;
    while (true)
    {
        if (curr_node->is_left_child())
        {
            ptr_ = curr_node->get_parent();
            return *this;
        }
        else if (curr_node->is_right_child())
        {
            curr_node = curr_node->get_parent();
        }
        else assert(0);
    }
    
    assert(0);
    return *this;
}

template <typename T>
typename BST<T>::BSTIterator BST<T>::BSTIterator::operator++(int)
{
    BSTIterator tmp{*this};
    operator++();
    return tmp;
}

template <typename T>
typename BST<T>::BSTIterator &BST<T>::BSTIterator::operator--()
{
    if (!ptr_) 
    { // suppose we are end()
        ptr_ = bst_ptr_.max_val_node_;
        return *this;
    };

    if (ptr_ == bst_ptr_.min_val_node_) return *this; // we are begin(), nowhere to decrement
    
    if (ptr_->get_left())
    {
        ptr_ = ptr_->get_left();
        while (ptr_->get_right()) ptr_ = ptr_->get_right();
        return *this;
    }

    BinNode *curr_node = ptr_;
    while (true)
    {
        if (curr_node->is_right_child())
        {
            ptr_ = curr_node->get_parent();
            return *this;
        }
        else if (curr_node->is_left_child())
        {
            curr_node = curr_node->get_parent();
        }
        else assert(0);
    }
    
    assert(0);
    return *this;
}

template <typename T>
typename BST<T>::BSTIterator BST<T>::BSTIterator::operator--(int)
{
    BSTIterator tmp{*this};
    operator--();
    return tmp;
}
