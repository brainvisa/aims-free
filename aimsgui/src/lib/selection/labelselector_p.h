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

#ifndef AIMS_SELECTION_LABELSELECTOR_P_H
#define AIMS_SELECTION_LABELSELECTOR_P_H


#include <aims/selection/labelselector.h>
#include <aims/listview/qatreewidget.h>
#include <QListWidget>

namespace aims
{
  namespace gui
  {
    class LabelSelector;
  }
}


class QTreeWidgetItem;


class SelectBox : public aims::gui::QATreeWidget
{
  Q_OBJECT

public:
  SelectBox( aims::gui::LabelSelector* ls, QWidget * parent=0, 
             const char * name=0 );
  virtual ~SelectBox();

  aims::SelectionSet selection() const;
  void addSelection( const aims::SelectionSet & );
  void addSelection( const aims::SelectionSet &, const std::string & parent );
  void mergeSelection( const aims::SelectionSet & );

public slots:
  void clearAll();

protected:
  virtual void dragEnterEvent( QDragEnterEvent* event );
  virtual void dragMoveEvent( QDragMoveEvent* event );
  virtual void dropEvent( QDropEvent* event );
  void insertUniqueItem( QTreeWidgetItem* parent, const QString & text );

protected slots:
  void deleteSelection();
  void rightButtonPressed( QTreeWidgetItem*, const QPoint & pos );

private:
  aims::gui::LabelSelector	*_labelsel;
};


class aims::gui::LabelSelector::ModelBox : public QListWidget
{
  Q_OBJECT

public:
  ModelBox( QWidget* parent = 0 );
  virtual ~ModelBox();

signals:
//   void itemRightPressed( QTreeWidgetItem *item, const QPoint & pos );
  void dragStart( QListWidgetItem*, Qt::MouseButtons state,
    Qt::KeyboardModifiers mod );

protected:
  virtual void mousePressEvent( QMouseEvent* event );
  virtual void mouseMoveEvent( QMouseEvent* event );
  virtual void mouseReleaseEvent( QMouseEvent* event );

private:
  struct Private;
  Private *d;
};

#endif

