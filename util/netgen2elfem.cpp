/****************************************************************/
/*        Converter from netgen neutral file to elfem-file      */
/***************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <list>
#include <vector>
#include <list>
#include <map>

//#define DEBUG_INPUT

using namespace std;

void readInput(vector<vector<double> > & vertices,
               vector<vector<long> > & cellsInSegment,
               long segmentnum, long dimension)
{
   long elementnum;
   //number of points:
   cin >> elementnum; 

   vertices.resize(elementnum);

   //read vertices:
   for (int i=0; i<elementnum; ++i)
   {
      vertices[i].resize(dimension);

      cin >> vertices[i][0];
      cin >> vertices[i][1];

      if (dimension == 3)
        cin >> vertices[i][2];
   }

   #ifdef DEBUG_INPUT
   cout << "Vertices read: " << std::endl;
   for (int i=0; i<elementnum; ++i)
   {
     cout << i << " " << vertices[i][0] << " " << vertices[i][1] << " ";
     if (dimension == 3)
       cout << vertices[i][2];
     cout << endl;
   }
   #endif

   //read cells:
   cin >> elementnum;
   cellsInSegment.reserve(elementnum);
   long segmentOfCell;
   std::vector<long> tempvec(dimension+1);

   for (int i=0; i<elementnum; ++i)
   {
      cin >> segmentOfCell;
      cin >> tempvec[0];
      cin >> tempvec[1];
      cin >> tempvec[2];
      if (dimension == 3)
	cin >> tempvec[3];

      if (segmentOfCell == segmentnum)
        cellsInSegment.push_back(tempvec);
   }

   #ifdef DEBUG_INPUT
   typedef vector<vector<long> >::iterator      CellIterator;
   typedef vector<long>::iterator               VertexIndexOnCellIterator;

   cout << "Cells read: " << elementnum << "/" << cellsInSegment.size() << std::endl;
   long counter = 0;
   for (CellIterator cit=cellsInSegment.begin();
         cit != cellsInSegment.end();
         ++cit)
   {
     cout << counter << " ";
     for (VertexIndexOnCellIterator viocit = cit->begin();
           viocit != cit->end();
           ++viocit)
     {
       cout << *viocit << " ";
     }
     cout << endl;
     ++counter;
   }
   #endif

}

void setBoundary(vector<vector<double> > & vertices, list<pair<long, double> > & bndlist)
{
  //some boundary setup here (if required)
}

void writeOutput(vector<vector<double> > & vertices,
                 vector<vector<long> > & cellsInSegment,
                 list<pair<long, double> > & bndlist)
{
   //step 1: Determine active vertices:
   typedef vector<vector<long> >::iterator      CellIterator;
   typedef vector<long>::iterator               VertexIndexOnCellIterator;
   typedef vector<double>::iterator             VertexCoordsIterator;

   map<long, bool> vertexActivity;

   for (CellIterator cit=cellsInSegment.begin();
         cit != cellsInSegment.end();
         ++cit)
   {
     for (VertexIndexOnCellIterator viocit = cit->begin();
           viocit != cit->end();
           ++viocit)
     {
       vertexActivity[*viocit] = true;
     }
   }

   //step 2: Write vertices and set up index map:
   map<long, long> vertexIndexMap;

   long activeVerticesCounter = 0;
   for (unsigned long i=1; i<=vertices.size(); ++i)
   {
      if (vertexActivity[i] == true)
        ++activeVerticesCounter;
   }

   cout << "Nodes: " << activeVerticesCounter << endl;
   long counter = 0;

   for (unsigned long i=0; i<vertices.size(); ++i)
   {
      if (vertexActivity[i+1])
      {
        cout << counter << " ";
        vertexIndexMap[i+1] = counter;
        ++counter;
        for (VertexCoordsIterator vcit = vertices[i].begin();
              vcit != vertices[i].end();
              ++vcit)
        {
           cout << *vcit << " ";
        }
        cout << endl;
      }
   }

   //step 3: Write cells:
   cout << "Cell-Type: Simplex" << endl;
   cout << "Cells: " << cellsInSegment.size() << endl;

   counter = 0;
   for (CellIterator cit=cellsInSegment.begin();
         cit != cellsInSegment.end();
         ++cit)
   {
     cout << counter << " ";
     ++counter;
     for (VertexIndexOnCellIterator viocit = cit->begin();
           viocit != cit->end();
           ++viocit)
     {
       cout << vertexIndexMap[*viocit] << " ";
     }
     cout << endl;
   }

   //step 4: Write boundary
   typedef list<pair<long, double> >::iterator        BoundaryIterator;

   cout << "Boundary Vertices: " << bndlist.size() << endl;
   for (BoundaryIterator bit = bndlist.begin();
        bit != bndlist.end();
        ++bit)
   {
      cout << vertexIndexMap[bit->first] << " " << bit->second << endl;
   }

};

int main(int argc, char *argv[])
{
   int dimension = 3;
   int segment = 1;
   vector<vector<double> >    vertices;
   vector<vector<long> >      cellsInSegment;
   list<pair<long, double> >  bndlist;

   if (argc > 1 && argv[1][0] == '2')
      dimension = 2;
   if (argc > 2)
      segment = argv[2][0] - '0';

   cout << "Dimension: " << dimension << endl;

   readInput(vertices, cellsInSegment, segment, dimension);
   setBoundary(vertices, bndlist);
   writeOutput(vertices, cellsInSegment, bndlist);

   return 0;
}

