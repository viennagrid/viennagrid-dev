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
            void insert_callback( container_collection_element_type & element, inserter_type & inserter)
            {
                element.insert_callback( inserter );
            }
            
            template<typename inserter_type>
            void insert_callback( bool & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( char & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( unsigned char & element, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( short & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( unsigned short & element, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( int & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( unsigned int & element, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( long & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( unsigned long & element, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( float & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( double & element, inserter_type & inserter) {}
            
            
            template<typename inserter_type>
            void insert_callback( const bool & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const char & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const unsigned char & element, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( const short & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const unsigned short & element, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( const int & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const unsigned int & element, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( const long & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const unsigned long & element, inserter_type & inserter) {}
            
            template<typename inserter_type>
            void insert_callback( const float & element, inserter_type & inserter) {}

            template<typename inserter_type>
            void insert_callback( const double & element, inserter_type & inserter) {}
        }
    }
}


#endif