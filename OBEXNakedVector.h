#pragma once

namespace OBEX
{
	template<class T>
	class NakedVector
	{
		public:
			NakedVector(int _reserve=64)
			{
				vec = new T[_reserve];
				capacity = _reserve;
				count = 0;
			}

			~NakedVector()
			{
				delete [] vec;
			}

			int Count() const
			{
				return count;
			}

			void Clear()
			{
				count = 0;
			}

			void Remove(int start, int end)
			{
				for(int i=start; i<count-1; i++)
				{
					vec[i] = vec[end+(i-start)+1];
				}

				count -= (end-start);
			}

			void RemoveAt(int index)
			{
				for(int i=index; i<count-1; i++)
				{
					vec[index] = vec[index+1];
				}

				count--;
			}

			void PushBack(const T& x)
			{
				if(count >= capacity)
					Resize();
				vec[count++] = x;
			}

			void PushBack(const NakedVector<T>& x)
			{
				while(true)
				{
					int free = capacity - count;
					int needed = x.Count();

					if(free >= needed)
						break;
					else
						Resize();
				}

				memcpy(vec+(sizeof(T)*count), x.Ptr(), sizeof(T)*x.Count());
				count += x.Count();
			}

			void PushBack(T* x, int length)
			{
				while(true)
				{
					int free = capacity - count;
					int needed = length;

					if(free >= needed)
						break;
					else
						Resize();
				}

				memcpy(vec+(sizeof(T)*count), x, sizeof(T)*length);
				count += length;
			}

			void PopBack()
			{
				count--;
			}

			T& operator[](int index)
			{
				return vec[index];
			}

			T* Ptr() const
			{
				return vec;
			}

		private:
			void Resize()
			{
				T* vec2 = new T[capacity*2];

				for(int i=0; i<count; i++)
					vec2[i] = vec[i];

				delete [] vec;
				vec = vec2;

				capacity *= 2;
			}

			T*		vec;
			int		count;
			int		capacity;
	};
}