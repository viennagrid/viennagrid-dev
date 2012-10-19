#ifndef VIENNAGRID_STORAGE_CONTAINER_COLLECTION_ELEMENT_HPP
#define VIENNAGRID_STORAGE_CONTAINER_COLLECTION_ELEMENT_HPP


namespace viennagrid
{
    namespace storage
    {
        namespace result_of
        {
            template<typename type>
            struct container_collection_element_required_typelist
            {
                typedef typename type::required_typelist typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<bool>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<char>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<unsigned char>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<short>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<unsigned short>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<int>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<unsigned int>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<long>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<unsigned long>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<float>
            {
                typedef viennameta::null_type typelist;                
            };
            
            template<>
            struct container_collection_element_required_typelist<double>
            {
                typedef viennameta::null_type typelist;                
            };
        }
        
        namespace container_collection_element
        {
            template<typename container_collection_element_type, typename inserter_type>
            void insert_callback( container_collection_element_type & element, bool insert_result, inserter_type & inserter)
            {
                element.insert_callback( inserter, insert_result );
            }
            
            template<typename container_collection_element_type, typename inserter_type>
            void insert_callback( const container_collection_element_type & element, bool insert_result, inserter_type & inserter)
            {
                element.insert_callback( inserter, insert_result );
            }
            
            template<typename inserter_type>
            void insert_callback( bool & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( char & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( unsigned char & element, bool insert_result, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( short & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( unsigned short & element, bool insert_result, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( int & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( unsigned int & element, bool insert_result, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( long & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( unsigned long & element, bool insert_result, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( float & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( double & element, bool insert_result, inserter_type & inserter) {}
            
            
            template<typename inserter_type>
            void insert_callback( const bool & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const char & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const unsigned char & element, bool insert_result, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( const short & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const unsigned short & element, bool insert_result, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( const int & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const unsigned int & element, bool insert_result, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( const long & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const unsigned long & element, bool insert_result, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( const float & element, bool insert_result, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const double & element, bool insert_result, inserter_type & inserter) {}
            
            
            
            
            

            
        }
    }
}


#endif
