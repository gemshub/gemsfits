#include <QKeyEvent>
#include <QMenu>
#include <QAction>
#include <QHeaderView>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include "json_view.h"
#include "CalcDialog.h"

namespace jsonui17 {

bool JsonView::expandedFields = true;

/// Internal structure to save expanded fields
class ExpandedLevelLine
{
public:

    QString keyname;           ///< Name of field
    ExpandedLevelLine *parent;
    std::vector<std::shared_ptr<ExpandedLevelLine>> children;

    ExpandedLevelLine( const QString& akey, ExpandedLevelLine* parentline ):
        keyname( akey ), parent( parentline )
    {}

    ~ExpandedLevelLine()
    {
        children.clear();
    }

    std::shared_ptr<ExpandedLevelLine> findKey( const QString& akey )
    {
        auto it = children.begin();
        while( it != children.end() )
        {
            if( it->get()->keyname == akey)
                return *it;
            it++;
        }
        return nullptr;
    }
};

//-------------------------------------------------------------------------------------
// class JsonView implements a tree view that displays items from a model to json data.
//-------------------------------------------------------------------------------------

JsonView::JsonView( QWidget * parent ):
    QTreeView( parent )
{
    setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    header()->setSectionsClickable(true);

    setEditTriggers( QAbstractItemView::DoubleClicked|QAbstractItemView::AnyKeyPressed );
    setFocusPolicy( Qt::StrongFocus );
    setTabKeyNavigation( false );
    setContextMenuPolicy( Qt::CustomContextMenu );
    setSelectionMode( QAbstractItemView::/*MultiSelection*/ExtendedSelection );
    setSelectionBehavior( QAbstractItemView::SelectItems );
    setIndentation( 30 );

    connect( this, &JsonView::customContextMenuRequested,
             this, &JsonView::slotPopupContextMenu);
    connect( header(), &QHeaderView::sectionClicked,
             this, &JsonView::changeCurrent);
}

void JsonView::changeCurrent( int section )
{
    if( !currentIndex().isValid() )
    {
        QModelIndex index = model()->index( 0, section, rootIndex() );
        setCurrentIndex(index);
    }
}

void JsonView::slotPopupContextMenu( const QPoint &pos )
{
    QModelIndex index = indexAt( pos );
    auto themodel = current_index_model( index );
    QMenu *menu = new QMenu(this);
    QAction* act;

    if(  themodel && themodel->isCanBeAdd(index) )
    {
        act =  new QAction(tr("Add &One Field ..."), this);
        act->setShortcut(tr("Ctr+O"));
        act->setStatusTip(tr("Add new line into object"));
        connect(act, &QAction::triggered, this, &JsonView::CmAddObject);
        menu->addAction(act);
    }

    if( themodel && themodel->isCanBeRemoved(index) )
    {
        act =  new QAction(tr("&Remove Selected Field"), this);
        act->setShortcut(tr("Ctr+R"));
        act->setStatusTip(tr("Delete current line from object"));
        connect(act, &QAction::triggered, this, &JsonView::CmDelObject);
        menu->addAction(act);
    }

    if(  themodel && themodel->isCanBeResized(index) )
    {
        menu->addSeparator();
        act =  new QAction(tr("Resize Selected List/Map/Set ..."), this);
        act->setShortcut(QKeySequence(Qt::Key_F4));
        act->setShortcutVisibleInContextMenu(true);
        act->setStatusTip(tr("Change current array size"));
        connect(act, &QAction::triggered, this, &JsonView::CmResizeArray);
        menu->addAction(act);
    }

    if(  themodel && themodel->isCanBeCloned( index ) )
    {
        act =  new QAction(tr("Clone Selected Field"), this);
        act->setShortcut(tr("Ctr+C"));
        act->setStatusTip(tr("Add copy of field to end of list/map"));
        connect(act, &QAction::triggered, this, &JsonView::CmCloneObject);
        menu->addAction(act);
    }

    if( index.column()==1 && themodel && themodel->isNumber(index) )
    {
        menu->addSeparator();
        act =  new QAction(tr("Calculator"), this);
        act->setShortcut(QKeySequence(Qt::Key_F5));
        //act->setShortcutVisibleInContextMenu(true);
        act->setStatusTip(tr("Use Calculator for specified cells"));
        connect(act, &QAction::triggered, this, &JsonView::CmCalc);
        menu->addAction(act);
    }

    if( themodel )
    {
        menu->addSeparator();
        act =  new QAction(tr("Copy Field &Path"), this);
        act->setShortcut(tr("Ctrl+P"));
        act->setStatusTip(tr("Copy selected field path"));
        connect(act, &QAction::triggered, this, &JsonView::CopyFieldPath);
        menu->addAction(act);

        act =  new QAction(tr("Copy &Field"), this);
        act->setShortcut(tr("Ctrl+F"));
        act->setStatusTip(tr("Copy selected field data"));
        connect(act, &QAction::triggered, this, &JsonView::CopyField);
        menu->addAction(act);

        act =  new QAction(tr("Paste Field &Value"), this);
        act->setShortcut(tr("Ctrl+V"));
        act->setShortcutVisibleInContextMenu(true);
        act->setStatusTip(tr("Paste data to field"));
        connect(act, &QAction::triggered, this, &JsonView::PasteField);
        menu->addAction(act);
    }

    menu->exec( viewport()->mapToGlobal(pos) );
    delete menu;
}

void JsonView::keyPressEvent(QKeyEvent* e)
{
    if ( e->modifiers() & Qt::ControlModifier )
    {
        switch ( e->key() )
        {
        case Qt::Key_R:
            CmDelObject();
            return;
        case Qt::Key_O:
            CmAddObject();
            return;
        case Qt::Key_P:
            CopyFieldPath();
            return;
        case Qt::Key_F:
            CopyField();
            return;
        case Qt::Key_V:
            PasteField();
            return;
        case Qt::Key_C:
            CmCloneObject();
            return;
        }
    }

    switch( e->key() )
    {
    case Qt::Key_F5:
        CmCalc();
        return;
    case Qt::Key_F4:
        CmResizeArray();
        return;
    }
    QTreeView::keyPressEvent(e);
}


void JsonView::CmCalc()
{
    QModelIndex index = currentIndex();
    auto* themodel = view_model();
    if( themodel && themodel->isNumber(index) &&  index.column()==1 )
    {
        CalcDialog calc( window() );
        if( calc.exec() )
        {
            QString res = calc.fun( index.data(Qt::EditRole).toDouble() );
            themodel->setData(index, res, Qt::EditRole);
        }
    }
}

void JsonView::CmAddObject()
{
    QModelIndex index = currentIndex();
    if( !index.isValid() )
        index = rootIndex();
    auto* themodel = view_model();

    try {
        if( themodel && themodel->isCanBeAdd(index) )
        {
            QModelIndex ndx = themodel->addObject( window(), index);
            selectionModel()->setCurrentIndex( ndx, QItemSelectionModel::ClearAndSelect );
        }
        else
            QMessageBox::information( window(), "Add to Object", "Please, determine object to be expanded" );

    }
    catch(TError& err)
    {
        QMessageBox::critical( this, err.title.c_str(), err.mess.c_str() );
    }
    catch(std::exception& e)
    {
        QMessageBox::critical( window(), "std::exception", e.what() );
    }
}


void JsonView::CmCloneObject()
{
    QModelIndex index = currentIndex();
    auto* themodel = view_model();

    try {
        if( index.isValid() &&  themodel && themodel->isCanBeCloned( index ) )
        {
            QModelIndex ndx = themodel->cloneObject( window(), index);
            selectionModel()->setCurrentIndex( ndx, QItemSelectionModel::ClearAndSelect );
        }
        else
            QMessageBox::information( window(), "Clone Object", "Please, determine object to be expanded" );

    }
    catch(TError& err)
    {
        QMessageBox::critical( this, err.title.c_str(), err.mess.c_str() );
    }
    catch(std::exception& e)
    {
        QMessageBox::critical( window(), "std::exception", e.what() );
    }
}

void JsonView::CmDelObject()
{
    QModelIndex index = currentIndex();
    auto* themodel = view_model();

    try {
        if(  index.isValid() && themodel && themodel->isCanBeRemoved(index) )
        {
            selectionModel()->clear();
            collapse(index);
            themodel->delObject( window(), index);
            //update();
        } else
            QMessageBox::information( window(), "Delete from Object", "Please, determine the key-value pair to be deleted" );

    }
    catch(TError& err)
    {
        QMessageBox::critical( this, err.title.c_str(), err.mess.c_str() );
    }
    catch(std::exception& e)
    {
        QMessageBox::critical( window(), "std::exception", e.what() );
    }
    catch(...)
    {
        QMessageBox::critical( window(), "std::exception", "Undefined exception" );
    }
}

void JsonView::CmResizeArray()
{
    QModelIndex index = currentIndex();
    auto* themodel = view_model();

    try {
        if(  index.isValid() && themodel && themodel->isCanBeResized(index) )
        {
            // collapse(index);
            themodel->resizeArray( window(), index);
        }
        else
            QMessageBox::information( window(), "Resize Array", "Please, determine arrays key to be resized" );

    }
    catch(std::exception& e)
    {
        QMessageBox::critical( window(), "std::exception", e.what() );
    }
}

void JsonView::CopyFieldPath()
{
    QModelIndex index = currentIndex();
    auto themodel = current_index_model( index );

    try {
        if(  themodel )
        {
            QString clipText = themodel->getFieldPath( index ).c_str();
            QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
        }
    }
    catch(TError& err)
    {
        QMessageBox::critical( this, err.title.c_str(), err.mess.c_str() );
    }
    catch(std::exception& e)
    {
        QMessageBox::critical( window(), "std::exception", e.what() );
    }
    catch(...)
    {
        QMessageBox::critical( window(), "std::exception", "Undefined exception" );
    }
}


void JsonView::CopyField()
{
    QModelIndex index = currentIndex();
    auto themodel = current_index_model( index );

    try {
        if(  themodel )
        {
            QString clipText = themodel->getFieldData( index ).c_str();
            QApplication::clipboard()->setText(clipText/*, QClipboard::Clipboard*/);
        }
    }
    catch(TError& err)
    {
        QMessageBox::critical( this, err.title.c_str(), err.mess.c_str() );
    }
    catch(std::exception& e)
    {
        QMessageBox::critical( window(), "std::exception", e.what() );
    }
    catch(...)
    {
        QMessageBox::critical( window(), "std::exception", "Undefined exception" );
    }
}

void JsonView::PasteField()
{
    QModelIndex index = currentIndex();
    auto* themodel = view_model();
    auto clipText = QApplication::clipboard()->text(QClipboard::Clipboard).toStdString();

    if( clipText.empty() || !index.isValid() || !themodel )
        return;

    // save old expand level
    int sec0size = 250, sec1size = 250;
    sec0size = header()->sectionSize(0);
    sec1size = header()->sectionSize(1);
    std::shared_ptr topList = std::make_shared<ExpandedLevelLine>( "root", nullptr );
    save_expanded_state( topList );

    try {
        themodel->setFieldData(index, clipText);
        restore_expanded_state(topList);
        header()->resizeSection(0, sec0size);
        header()->resizeSection(1, sec1size);
    }
    catch(TError& err)
    {
        restore_expanded_state(topList);
        header()->resizeSection(0, sec0size);
        header()->resizeSection(1, sec1size);
        QMessageBox::critical(this, err.title.c_str(), err.mess.c_str() );
    }
    catch(std::exception& e)
    {
        restore_expanded_state(topList);
        header()->resizeSection(0, sec0size);
        header()->resizeSection(1, sec1size);
        QMessageBox::critical( window(), "std::exception", e.what() );
    }
    catch(...)
    {
        restore_expanded_state(topList);
        header()->resizeSection(0, sec0size);
        header()->resizeSection(1, sec1size);
        QMessageBox::critical( window(), "std::exception", "Undefined exception" );
    }

}

// Save expand level  -----------------------------------------

void JsonView::updateModelData(const std::string& json_string)
{
    auto* themodel = view_model();
    if( !themodel )
        return;

    // save old expand level
    int sec0size = 250, sec1size = 250;
    std::shared_ptr<ExpandedLevelLine> topList;

    if( expandedFields )
    {
        sec0size = header()->sectionSize(0);
        sec1size = header()->sectionSize(1);
        topList = std::make_shared<ExpandedLevelLine>( "root", nullptr );
        save_expanded_state(topList);
    }

    // update model
    themodel->setupModelData(json_string);

    // restore expand level
    if( expandedFields )
        restore_expanded_state(topList);
    else
        expandToDepth(1);
    header()->resizeSection(0, sec0size);
    header()->resizeSection(1, sec1size);
}

void JsonView::resetModel()
{
    auto* themodel = view_model();
    if( !themodel )
        return;

    // save old expand level
    int sec0size = 250, sec1size = 250;
    std::shared_ptr<ExpandedLevelLine> topList;

    if( expandedFields )
    {
        sec0size = header()->sectionSize(0);
        sec1size = header()->sectionSize(1);
        topList = std::make_shared<ExpandedLevelLine>("root", nullptr);
        save_expanded_state( topList );
    }

    // reset model
    themodel->resetModelData();

    // restore expand level
    if( expandedFields )
        restore_expanded_state( topList );
    else
        expandToDepth(1);
    header()->resizeSection(0, sec0size);
    header()->resizeSection(1, sec1size);
}

void JsonView::save_expanded_state(std::shared_ptr<ExpandedLevelLine> topList)
{
    for(int row = 0; row < model()->rowCount(); ++row)
        save_expanded_on_level( model()->index(row,0), topList);
}

void JsonView::restore_expanded_state( std::shared_ptr<ExpandedLevelLine> topList)
{
    setUpdatesEnabled(false);
    for(int row = 0; row < model()->rowCount(); ++row)
        restore_expanded_on_level(model()->index(row,0), topList);
    setUpdatesEnabled(true);
}

void JsonView::save_expanded_on_level(const QModelIndex& index,  std::shared_ptr<ExpandedLevelLine> parent)
{
    if( index.isValid() && isExpanded(index))
    {
        QString key = index.data(Qt::EditRole).toString();
        std::shared_ptr child = std::make_shared<ExpandedLevelLine>(key, parent.get());
        if(parent != nullptr) {
            parent->children.push_back(child);
        }
        for(int row = 0; row < model()->rowCount(index); ++row) {
            save_expanded_on_level( index.model()->index(row,0, index), child );
        }
    }
}

void JsonView::restore_expanded_on_level(const QModelIndex& index,  std::shared_ptr<ExpandedLevelLine> topData)
{
    if( index.isValid() )
    {
        QString key = index.data(Qt::EditRole).toString();
        auto child = topData->findKey(key);
        if(child != nullptr )
        {
            setExpanded(index, true);
            for(int row = 0; row < model()->rowCount(index); ++row)
                restore_expanded_on_level(index.model()->index(row, 0, index), child);
        }
    }
}

} // namespace jsonui17

