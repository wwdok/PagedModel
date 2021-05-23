#include "filter.h"

Filter::Filter(QObject *parent) : QSortFilterProxyModel(parent)
{
    //下面三个参数的含义可以看我画的示意图;https://pic.imgdb.cn/item/60aa259135c5199ba7b1bff4.jpg
    total_added = 0; //当前页面已加载的条目，该值不会超过页面条目容量
    total_passed = 0; //代表已通过筛选条件的条目数量
    total_processed = 0;//代表通过筛选条件和没通过筛选条件所有的条目数量
    total_pages = 0;
    regxpattern = "";
//    pagingdata = true;//是否对数据应用分页效果
//    pagesize = 10;
//    currentpage = 0;
}

bool Filter::pageData()
{
    return pagingdata;
}

void Filter::setPageData(bool value)
{
    pagingdata = value;
    qDebug() << "Page data: " << pagingdata;
}

int Filter::pageSize()
{
    return pagesize;
}

void Filter::setPageSize(int value)
{
    pagesize = value;
    qDebug() << "Page size: " << pagesize;
}

int Filter::pageCount()
{
    if(!pagingdata) return 1;
    return total_pages;
}

void Filter::filterPaged(QString pattern)
{
    regxpattern = pattern;
    search();
}

int Filter::currentPage()
{
    return currentpage;
}

void Filter::setCurrentPage(int value)
{
    currentpage = value;
    search();
}

void Filter::back()
{
    currentpage--;
    if(currentpage < 0) currentpage = 0;
    search();
}

void Filter::next()
{
    currentpage++;
    if(currentpage > total_pages) currentpage = total_pages;
    search();
}

void Filter::search()
{

    total_added = 0;
    total_passed = 0;
    total_processed = 0;
    total_pages = 0;
    min = currentpage * pagesize;
    max = min + pagesize;
    emit started();
    setFilterRegularExpression(regxpattern);
}

bool Filter::process(bool allowed, bool countonly, QString reason) const
{
    total_processed++;
    if(allowed == true && countonly == false) total_added++;
    if(countonly)
    {
        total_passed++;
        total_pages = total_passed / pagesize;
    }
    qDebug() << "Allowed: " << allowed << reason;
    //see if we are done
    //qDebug() << total_processed << " of " << sourceModel()->rowCount();
    if(total_processed >= sourceModel()->rowCount()) emit finished();
    return allowed;
}

bool Filter::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    qDebug()<<"filterAcceptsRow was called";
    //Get the index of the item
    QModelIndex index = sourceModel()->index(source_row,filterKeyColumn(),source_parent);

    //qDebug() << "Count:  " << rowCount() << "Added: " << total_added << "Processed: " << total_processed << " Data: " << sourceModel()->data(index).toString();

    //Make sure it meets our filter
    if(!sourceModel()->data(index).toString().contains(filterRegularExpression())) return process(false,false,"Failed filter");

    //If we are here, it means this row meet the filter

    //If not paging the data
    if(!pagingdata) return process(true,true,"Not paging");

    //If we are here, we are paging the results!!!

    //Deny anything outside the page range
    if(total_added >= pagesize) return process(false, true,"Not in page range");

    //If we are here, its passed the filter and we can still add to the page!!!

    //Make sure its in the current page, mainly this lead to the effect of what we see in the screen.
    if(total_passed >= min && total_passed < max) return process(true, true,"In page range");

    return process(false, true,"Default");
}
