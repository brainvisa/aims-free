/* This software and supporting documentation are distributed by
*     Institut Federatif de Recherche 49
*     CEA/NeuroSpin, Batiment 145,
*     91191 Gif-sur-Yvette cedex
*     France
*
* This software is governed by the CeCILL-B license under
* French law and abiding by the rules of distribution of free software.
* You can  use, modify and/or redistribute the software under the
* terms of the CeCILL-B license as circulated by CEA, CNRS
* and INRIA at the following URL "http://www.cecill.info".
*
* As a counterpart to the access to the source code and  rights to copy,
* modify and redistribute granted by the license, users are provided only
* with a limited warranty  and the software's author,  the holder of the
* economic rights,  and the successive licensors  have only  limited
* liability.
*
* In this respect, the user's attention is drawn to the risks associated
* with loading,  using,  modifying and/or developing or reproducing the
* software by the user in light of its specific status of free software,
* that may mean  that it is complicated to manipulate,  and  that  also
* therefore means  that it is reserved for developers  and  experienced
* professionals having in-depth computer knowledge. Users are therefore
* encouraged to load and test the software's suitability as regards their
* requirements in conditions enabling the security of their systems and/or
* data to be ensured and,  more generally, to use and operate it in the
* same conditions as regards security.
*
* The fact that you are presently reading this means that you have had
* knowledge of the CeCILL-B license and that you accept its terms.
*/

#include <cstdlib>
#include <iostream>
#include <memory>

#include <cartobase/exception/assert.h>

#include <aims/transformation/affinetransformation3d.h>
#include <aims/transformation/transformation_chain.h>


using namespace std;
using namespace carto;
using namespace aims;

// Approximate equality of two Point3df
#define ASSERT_EQ( P1, P2 ) (                             \
    carto::test_assert(((P1) - (P2)).norm() < 1e-5,       \
                       #P1 " ≈ " #P2, __FILE__, __LINE__) \
)


int main()
{
  static const Point3df p1(0, 0, 0), p2(1, 0, 0), p3(0, 1, 0), p4(0, 0, 1);

  static const float trm_array[] = {
    0.0f, 0.5f, 0.0f, 0.1f,
    1.0f, 0.0f, 0.0f, 0.2f,
    0.0f, 0.0f, 2.0f, 0.3f,
    0.0f, 0.0f, 0.0f, 1.0f};
  static const vector<float> trm_vec(trm_array, trm_array + 16);
  static const Point3df tp1(0.1f, 0.2f, 0.3f), tp2(0.1f, 1.2f, 0.3f),
    tp3(0.6f, 0.2f, 0.3f), tp4(0.1f, 0.2f, 2.3f);


  cout << "Testing AffineTransformation3d..." << endl;
  {
    cout << "  default constructor" << endl;
    const AffineTransformation3d id;
    ASSERT_EQ(id.transform(p1), p1); ASSERT_EQ(id.transform(p2), p2);
    ASSERT_EQ(id.transform(p3), p3); ASSERT_EQ(id.transform(p4), p4);
    ASSERT(id.isIdentity());

    cout << "  vector<float> constructor" << endl;
    const AffineTransformation3d t1(trm_vec);
    ASSERT_EQ(t1.transform(p1), tp1); ASSERT_EQ(t1.transform(p2), tp2);
    ASSERT_EQ(t1.transform(p3), tp3); ASSERT_EQ(t1.transform(p4), tp4);

    cout << "  copy constructor" << endl;
    AffineTransformation3d t2(t1);
    ASSERT_EQ(t2.transform(p1), tp1); ASSERT_EQ(t2.transform(p2), tp2);
    ASSERT_EQ(t2.transform(p3), tp3); ASSERT_EQ(t2.transform(p4), tp4);

    cout << "  assignment operator" << endl;
    t2 = id;
    ASSERT_EQ(t2.transform(p1), p1); ASSERT_EQ(t2.transform(p2), p2);
    ASSERT_EQ(t2.transform(p3), p3); ASSERT_EQ(t2.transform(p4), p4);

    cout << "  invertible(), inverse(), and getInverse()" << endl;
    ASSERT(t1.invertible());
    AffineTransformation3d t3(*t1.inverse());
    ASSERT_EQ(t3.transform(tp1), p1); ASSERT_EQ(t3.transform(tp2), p2);
    ASSERT_EQ(t3.transform(tp3), p3); ASSERT_EQ(t3.transform(tp4), p4);

    unique_ptr<Transformation> t4(t1.getInverse());
    Transformation3d *t5 = dynamic_cast<Transformation3d *>( t4.get() );
    ASSERT_EQ(t5->transform(tp1), p1); ASSERT_EQ(t5->transform(tp2), p2);
    ASSERT_EQ(t5->transform(tp3), p3); ASSERT_EQ(t5->transform(tp4), p4);
  }

  cout << "Testing TransformationChain3d..." << endl;
  {
    cout << "  default constructor" << endl;
    const TransformationChain3d t1;
    ASSERT(t1.isIdentity());
    ASSERT(t1.invertible());

    cout << "  transform with an empty chain" << endl;
    ASSERT_EQ(t1.transform(p1), p1); ASSERT_EQ(t1.transform(p2), p2);
    ASSERT_EQ(t1.transform(p3), p3); ASSERT_EQ(t1.transform(p4), p4);

    const const_ref<AffineTransformation3d> trm(
      new AffineTransformation3d(trm_vec));
    cout << "  push_back a single element" << endl;
    TransformationChain3d t2;
    t2.push_back(trm);
    ASSERT(!t2.isIdentity());
    ASSERT_EQ(t2.transform(p1), tp1); ASSERT_EQ(t2.transform(p2), tp2);
    ASSERT_EQ(t2.transform(p3), tp3); ASSERT_EQ(t2.transform(p4), tp4);

    cout << "  pop_back" << endl;
    t2.pop_back();
    ASSERT(t2.isIdentity());

    cout << "  push_front a single element" << endl;
    t2.push_front(trm);
    ASSERT(!t2.isIdentity());
    ASSERT_EQ(t2.transform(p1), tp1); ASSERT_EQ(t2.transform(p2), tp2);
    ASSERT_EQ(t2.transform(p3), tp3); ASSERT_EQ(t2.transform(p4), tp4);

    cout << "  pop_front" << endl;
    t2.pop_front();
    ASSERT(t2.isIdentity());

    cout << "  chaining two transformations" << endl;
    rc_ptr<TransformationChain3d> t3(new TransformationChain3d);
    t3->push_back(trm);
    const const_ref<AffineTransformation3d> trm2(
      new AffineTransformation3d(-AffineTransformation3d()));
    t3->push_back(trm2);
    ASSERT_EQ(t3->transform(p1), -tp1); ASSERT_EQ(t3->transform(p2), -tp2);
    ASSERT_EQ(t3->transform(p3), -tp3); ASSERT_EQ(t3->transform(p4), -tp4);

    cout << "  inversion of two-transformation chain" << endl;
    ASSERT(!t3->isIdentity());
    ASSERT(t3->invertible());
    rc_ptr<Transformation> t4(t3->getInverse());
    Transformation3d *t10 = dynamic_cast<Transformation3d *>( t4.get() );
    ASSERT_EQ(t10->transform(-tp1), p1); ASSERT_EQ(t10->transform(-tp2), p2);
    ASSERT_EQ(t10->transform(-tp3), p3); ASSERT_EQ(t10->transform(-tp4), p4);

    cout << "  simplification of empty chain" << endl;
    const const_ref<Transformation3d> t5(t1.simplify());
    ASSERT(t5->isIdentity());

    t2.push_back(trm);
    cout << "  simplification of single-element chain" << endl;
    const const_ref<Transformation3d> t6(t2.simplify());
    ASSERT_EQ(t6->transform(p1), tp1); ASSERT_EQ(t6->transform(p2), tp2);
    ASSERT_EQ(t6->transform(p3), tp3); ASSERT_EQ(t6->transform(p4), tp4);
    ASSERT(dynamic_cast<const TransformationChain3d*>(t6.pointer()) == 0);
    ASSERT(*dynamic_cast<const AffineTransformation3d*>(t6.pointer())
           == *trm);

    cout << "  simplification of a chain of two AffineTransformation3d" << endl;
    const const_ref<Transformation3d> t7(t3->simplify());
    ASSERT_EQ(t7->transform(p1), -tp1); ASSERT_EQ(t7->transform(p2), -tp2);
    ASSERT_EQ(t7->transform(p3), -tp3); ASSERT_EQ(t7->transform(p4), -tp4);
    const AffineTransformation3d * affine_t7
      = dynamic_cast<const AffineTransformation3d*>(t7.pointer());
    ASSERT(affine_t7 != 0);
    ASSERT(*affine_t7 == *trm2 * *trm);

    cout << "  simplification of a chain containing sub-chains" << endl;
    TransformationChain3d t8;
    t8.push_back(t3);
    t8.push_back(const_ref<Transformation3d>(new TransformationChain3d));
    t8.push_back(trm);
    const const_ref<Transformation3d> t9(t8.simplify());
    const AffineTransformation3d * affine_t9
      = dynamic_cast<const AffineTransformation3d*>(t9.pointer());
    ASSERT(affine_t9 != 0);
    ASSERT(*affine_t9 == *trm * *trm2 * *trm);
}

  return EXIT_SUCCESS;
}
