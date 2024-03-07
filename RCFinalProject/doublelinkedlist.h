#pragma once

/************************************************************************************
 2024 (C) Renzo Calderon
*************************************************************************************/

// class template definition for double linked list and node

template <class T>
struct SDoubleLinkedListNode
{
	T m_data = nullptr;
	SDoubleLinkedListNode<T>* m_prevPtr = nullptr;
	SDoubleLinkedListNode<T>* m_nextPtr = nullptr;

	SDoubleLinkedListNode(T data)
	{
		m_data = data;
	}
};

template <class T>
class CDoubleLinkedList
{
public:
	~CDoubleLinkedList()
	{
		// destroy all nodes
		SDoubleLinkedListNode<T>* ptr = this->m_head;
		while (ptr != nullptr)
		{
			SDoubleLinkedListNode<T>* nextPtr = ptr->m_nextPtr;
			delete ptr;
			ptr = nextPtr;
		}
	}
		
	SDoubleLinkedListNode<T>* RemoveNode(SDoubleLinkedListNode<T>* nodePtr)
	{
		if (this->m_head != nullptr) // is there at least 1 node in the list?
		{
			SDoubleLinkedListNode<T>* ptr = this->m_head;
			while (ptr != nullptr)
			{
				if (ptr == nodePtr)
				{
					// grab next node
					SDoubleLinkedListNode<T>* nextNode = ptr->m_nextPtr;

					// handle previous node
					if (ptr->m_prevPtr == nullptr)
					{
						// this is the head node

						// make the next ptr the head ptr
						this->m_head = ptr->m_nextPtr;
						// if the next ptr is a valid node, then set its previous ptr to nullptr
						if (this->m_head != nullptr) 
						{
							this->m_head->m_prevPtr = nullptr;
						}
					}
					else 
					{
						// there is a valid previous pointer
						ptr->m_prevPtr->m_nextPtr = ptr->m_nextPtr;
					}

					// handle next node - if the next ptr is a valid node, then set its previous ptr to this node's previous pointer
					if (ptr->m_nextPtr != nullptr)
					{
						ptr->m_nextPtr->m_prevPtr = ptr->m_prevPtr;
					}

					delete ptr;
					return nextNode;
				}
				else
				{
					ptr = ptr->m_nextPtr;
				}
			}
		}

		return nullptr;
	}

	SDoubleLinkedListNode<T>* GetNextNode(SDoubleLinkedListNode<T>* nodePtr)
	{
		return nodePtr == nullptr ? nullptr : nodePtr->m_nextPtr;
	}

	SDoubleLinkedListNode<T>* FindNode(T data)
	{
		if (this->m_head != nullptr) // is there at least 1 node in the list?
		{
			SDoubleLinkedListNode<T>* ptr = this->m_head;
			while (ptr != nullptr)
			{
				if (ptr->m_data == data)
				{
					return ptr;
				}
				ptr = ptr->m_nextPtr;
			}
		}

		return nullptr;
	}

	bool IsEmpty() { return m_head == nullptr; }

	SDoubleLinkedListNode<T>* AddElement(T data)
	{
		if (data != nullptr) // is the data valid?
		{
			// move to the tail of the list
			SDoubleLinkedListNode<T>* ptr = this->m_head;
			while (ptr != nullptr && ptr->m_nextPtr != nullptr)
			{
				ptr = ptr->m_nextPtr;
			}

			// create the new node
			SDoubleLinkedListNode<T>* newNode = new SDoubleLinkedListNode<T>(data);
			newNode->m_prevPtr = ptr;
			newNode->m_nextPtr = nullptr;

			// insert the item at the end of the list
			if (this->m_head == nullptr)
			{
				this->m_head = newNode;
			}
			else
			{
				ptr->m_nextPtr = newNode;
			}

			return newNode;
		}

		return nullptr;
	}

	T GetHeadElement() 
	{ 
		if (m_head != nullptr)
		{
			return m_head->m_data;
		}
		return nullptr;
	}

	T RemoveElement(T element)
	{
		SDoubleLinkedListNode<T>* node = FindNode(element);
		if (node != nullptr)
		{
			SDoubleLinkedListNode<T>* nextNode = RemoveNode(node);
			return nextNode == nullptr ? nullptr : nextNode->m_data;
		}

		return nullptr;
	}

	T GetNextElement(T element)
	{
		SDoubleLinkedListNode<T>* node = FindNode(element);
		if (node != nullptr)
		{
			SDoubleLinkedListNode<T>* nextNode = GetNextNode(node);
			return nextNode == nullptr ? nullptr : nextNode->m_data;
		}

		return nullptr;
	}

private:
	SDoubleLinkedListNode<T>* m_head = nullptr;
};