#ifndef LIBDOM_DOM_HPP
#define LIBDOM_DOM_HPP

#include "dom_schema.hpp"

namespace dom {

	class IReducer
	{
	public:
		virtual int apply() = 0;
	};

	template<typename T>
	class array
	{
	public:

		struct view
		{
			array<T>* source = nullptr;
			std::function<bool(T&)> selector;
			std::function<bool(T&)> reducePred;
		};

		class reduce_view : public IReducer
		{
		public:
			array<T>* source = nullptr;
			std::function<int(T&)> reducePred;

			int apply() override
			{
				int count = 0;
				for (auto& v : source->container)
				{
					auto ret = reducePred(*v);
					count += ret;
				}

				return count;
			}
		};

	public:

        T* at(int index) const
		{
			return container.at(index);
		}

		void notifyDataChanged(T* value)
		{

		}

		void subscribe(array<T>* observer)
		{
			subs.push_back(observer);
		}

		void operator=(view v)
		{
			// Assigning a view to the array means we become bound to the source array, based on predicate
			sourceView = v;

			// subscribe to the other to get their updates
			v.source->subscribe(this);

			if (v.selector)
			{
				for (auto& sourceValue : v.source->container)
				{
					if (v.selector(*sourceValue))
					{
						container.push_back(sourceValue);
					}
				}
			}

			if (v.reducePred)
			{
				applyReduce();
			}
		}

		void applyReduce()
		{
			container.clear();

			int count = 0;
			for (auto& v : sourceView.source->container)
			{
				if (sourceView.reducePred(*v))
					count++;
			}

		}

		typename std::vector<T*>::iterator begin()
		{
			return container.begin();
		}

		typename std::vector<T*>::iterator end()
		{
			return container.end();
		}

		void push(T* value)
		{
			container.push_back(value);

			// notify subscribers
			for (auto& s : subs)
			{
				s->pushInternal(value);
			}
		}

		void pushInternal(T* value)
		{
			// The observed container has just pushed something to us
			if (sourceView.selector(*value))
			{
				container.push_back(value);
			}
		}

        int size() const
		{
			return container.size();
		}

		view select(std::function<bool(T&)> pred)
		{
			// Returns a view over THIS container, based on the predicate
			view v;
			v.source = this;
			v.selector = pred;
			return v;
		}

		std::shared_ptr<reduce_view> reduce(std::function<int(T&)> pred)
		{
			std::shared_ptr<reduce_view> v = std::make_shared<reduce_view>();
			v->source = this;
			v->reducePred = pred;
			return v;
		}

	private:
		std::vector<T*>        container;
		std::vector<array<T>*> subs;
		view                   sourceView;
	};


	template<typename T>
	class property
	{
	public: //API

        explicit property()
        {

        }

        explicit property(std::function<void()> listener)
        {
            dataChanged = listener;
        }

        std::function<void()> dataChanged;

		void set(T newValue)
		{
			_val = newValue;
            if (dataChanged) dataChanged();

			dispatchNewValue(newValue, nullptr);

			if (source)
			{
				source->dispatchNewValue(newValue, this);
			}
		}

	public:

		void dispatchNewValue(T value, property<T>* ignore)
		{
			_val = value;
            if (dataChanged) dataChanged();

			for (auto& s : subscribers)
			{
				if (s != ignore)
				{
					s->_val = value;
                    if (s->dataChanged) s->dataChanged();
				}
			}
		}

		void applyReduce()
		{
			int count = 0;
			for (auto& v : sourceView.source->container)
			{
				if (sourceView.reducePred(*v))
					count++;
			}
			_val = count;
            if (dataChanged) dataChanged();
		}

		void operator=(std::shared_ptr<IReducer> reduceView)
		{
			// sourceView = view;

			_val = reduceView->apply();
            if (dataChanged) dataChanged();
		}

		void operator=(property<T>& value)
		{
			// Create the channel
			source = &value;
			value.subscribers.push_back(this);

			_val = source->_val;
            if (dataChanged) dataChanged();
		}

		bool operator==(T other)
		{
			return _val == other;
		}

		T& operator*() {
			return _val;
		}

		T _val;
		property<T>* source = nullptr;
		std::vector<property<T>*> subscribers;
		typename array<T>::view sourceView;
	};


	template<typename T>
	class object
	{
	public:

		T* operator->()
		{
			return _val;
		}

		T* _val = new T();
	};

}


#endif //LIBDOM_DOM_HPP
