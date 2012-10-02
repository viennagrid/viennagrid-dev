#ifndef VIENNAGRID_STORAGE_IO_HPP
#define VIENNAGRID_STORAGE_IO_HPP

#include <ostream>
#include <iterator>

#include "container_collection.hpp"
#include "algorithm.hpp"

namespace viennagrid
{
    
    namespace storage
    {
        
        class container_output_functor
        {
        public:
            container_output_functor(std::ostream & _stream,
                                     const std::string & _container_delimiter = "\n",
                                     const std::string & _element_delimiter = " ") :
                                container_delimiter(_container_delimiter), element_delimiter(_element_delimiter), stream(_stream) {}
            
            template<typename container_type>
            void operator() (const container_type & container)
            {
                stream << typeid(container).name() << " [size=" << container.size() << "] ";
                std::copy( container.begin(), container.end(), std::ostream_iterator<typename container_type::value_type>(stream, element_delimiter.c_str())  );
                stream << container_delimiter;
            }
            
        private:
            std::string container_delimiter;
            std::string element_delimiter;
            std::ostream & stream;
        };
        
        
        template <typename container_typelist>
        std::ostream & operator<<(std::ostream & os, const collection_t<container_typelist> & container_collection)
        {
            container_output_functor f(os);
            
            viennagrid::storage::collection::for_each( container_collection, f );
            
            return os;
        }


    }
    
}

#endif
