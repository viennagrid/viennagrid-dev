#ifndef VIENNAGRID_COLLECTION_HPP
#define VIENNAGRID_COLLECTION_HPP

#include "TypeManip.h"
#include "Typelist.h"

namespace viennagrid
{
    namespace collection
    {
        
        template<typename type1, typename type2>
        struct meta_is_equal
        {
            static const bool value = false;
        };
        
        template<typename type>
        struct meta_is_equal<type,type>
        {
            static const bool value = true;
        };
        
        template<bool condition, typename type1, typename type2>
        struct meta_if
        {};
        
        template<typename type1, typename type2>
        struct meta_if<true, type1, type2>
        {
            typedef type1 result;
        };
        
        template<typename type1, typename type2>
        struct meta_if<false, type1, type2>
        {
            typedef type2 result;
        };
        
        
        
        
        // TODO: check that no 2 container with same value_type are present within a collection
        
        template <class TList, class T> struct ContainerOfValuetype;
        
        template <class T>
        struct ContainerOfValuetype<Loki::NullType, T>
        {
            typedef Loki::NullType result;
        };
        

        template <class head, class tail, class T>
        struct ContainerOfValuetype< Loki::Typelist<head, tail>, T >
        {
            typedef typename meta_if<
                meta_is_equal<T, typename head::value_type>::value,
                head,
                typename ContainerOfValuetype<tail,T>::result
                    >::result result;
        };
        
        
        template<typename typelist>
        class collection_layer;

        template<typename typelist>
        class collection_t : public collection_layer<typelist>
        {
        public:
            typedef typelist container_list;
            
            typedef collection_layer<typelist> base;
            
            using base::get;
            
            
            template<typename element_type>
            element_type * push_back( const element_type & element)
            {
                element_type * element_ptr = base::push_back(element);
                element_ptr->fill( *this );
                return element_ptr;
            }
            
        private:
        };
        
        

        template<typename head, typename tail>
        class collection_layer< Loki::Typelist<head, tail> > : public collection_layer<tail>
        {
        public:
            typedef collection_layer< tail > base;
            typedef typename head::value_type element_type;
            
            using base::get;
            head & get( head ) { return container; }
            const head & get( head ) const { return container; }
            
        protected:
            using base::push_back;
            element_type * push_back(const element_type & element)
            {
                container.push_back(element);
                container.back().id( container.size()-1 );
                
                return &container.back();
            }
            
            
            
        private:
            head container;
        };

        template<>
        class collection_layer< Loki::NullType >
        {
        public:            
            void get();
            void push_back();
        };
        
        
        
        
        template<typename collection_type, typename value_type>
        struct container_of
        {
            typedef typename ContainerOfValuetype< typename collection_type::container_list, value_type >::result type;
        };
        
        
        
        template<typename type, typename collection_type>
        typename ContainerOfValuetype< typename collection_type::container_list, type >::result & get( collection_type & c )
        {
            typedef typename ContainerOfValuetype< typename collection_type::container_list, type >::result container_type;
            return c.get( container_type() );
        }
        
        template<typename type, typename collection_type>
        const typename ContainerOfValuetype< typename collection_type::container_list, type >::result & get( const collection_type & c )
        {
            typedef typename ContainerOfValuetype< typename collection_type::container_list, type >::result container_type;
            return c.get( container_type() );
        }
        
        
        template<typename typelist>
        struct result_of
        {
            typedef collection_t<typename Loki::TL::NoDuplicates< typelist >::Result > type;
        };
        
    }

}

#endif