/* This software and supporting documentation are distributed by
 *     Institut Federatif de Recherche 49
 *     CEA/NeuroSpin, Batiment 145,
 *     91191 Gif-sur-Yvette cedex
 *     France
 *
 * This software is governed by the CeCILL license version 2 under
 * French law and abiding by the rules of distribution of free software.
 * You can  use, modify and/or redistribute the software under the
 * terms of the CeCILL license version 2 as circulated by CEA, CNRS
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
 * knowledge of the CeCILL license version 2 and that you accept its terms.
 */

#ifndef AIMS_SELECTION_LABELSELECTION_H
#define AIMS_SELECTION_LABELSELECTION_H


#include <aims/selection/selection.h>
#include <qdialog.h>
#include <iostream>

class SelectBox;
class Graph;
class QTreeWidgetItem;
class QListWidgetItem;

namespace aims
{

  class Hierarchy;
  class SelectionExpander;

  namespace gui
  {

    class LabelSelector : public QDialog
    {
      Q_OBJECT

      struct LabelSelector_private;
      friend class ::SelectBox;
      class ModelBox;

    public:
      LabelSelector( QWidget* parent = 0, const char* name = 0, 
                     bool modal=true, Qt::WindowFlags f=Qt::WindowFlags() );
      virtual ~LabelSelector();

      void loadPreselection( const QString & );
      void loadHierarchy( const QString & );
      void loadModel( const QString & );
      void printSelection( std::ostream & ostr = std::cout ) const;
      aims::SelectionSet selection() const;
      void showTab( const std::string & );

    public slots:
      void loadPreselection();
      void activatePreselection();
      void activateAddPreselection();
      void loadHierarchy();
      /** Tries to make/activate a selection from whatever is available 
          (model/hierarchy/preselection) */
      void autoActivate();
      void check();

    protected slots:
      virtual void accept();
      void loadModel();
      void selectModel();
      void selectModelPlusSel();

    private slots:
      void updateHierarchyView();
      void startDrag( QTreeWidgetItem*, Qt::MouseButtons,
                      Qt::KeyboardModifiers );
      void startDragModel( QListWidgetItem *, Qt::MouseButtons,
                           Qt::KeyboardModifiers );

    private:
      LabelSelector_private	*d;

      void updateModelCaps();
      Hierarchy *currentHierarchy();
      Graph* model();
      const SelectionExpander & expander() const;
    };

  }

}

#endif

