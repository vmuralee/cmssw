#include <stdexcept>
#include <cstdlib>
#include <string>
#include <cstring>
#include "OnlineDB/Oracle/interface/Oracle.h"

#include "OnlineDB/EcalCondDB/interface/ODFEPedestalOffsetInfo.h"

using namespace std;
using namespace oracle::occi;

ODFEPedestalOffsetInfo::ODFEPedestalOffsetInfo()
{
  m_env = nullptr;
  m_conn = nullptr;
  m_writeStmt = nullptr;
  m_readStmt = nullptr;
  m_config_tag="";
   m_ID=0;
   m_version=0;
   clear();   
}


void ODFEPedestalOffsetInfo::clear(){

}



ODFEPedestalOffsetInfo::~ODFEPedestalOffsetInfo()
{
}



int ODFEPedestalOffsetInfo::fetchNextId()  noexcept(false) {

  int result=0;
  try {
    this->checkConnection();

    m_readStmt = m_conn->createStatement(); 
    m_readStmt->setSQL("select COND2CONF_INFO_SQ.NextVal from DUAL ");
    ResultSet* rset = m_readStmt->executeQuery();
    while (rset->next ()){
      result= rset->getInt(1);
    }
    result++;
    m_conn->terminateStatement(m_readStmt);
    return result; 

  } catch (SQLException &e) {
    throw(std::runtime_error("ODFEPedestalOffsetInfo::fetchNextId():  "+e.getMessage()));
  }

}

void ODFEPedestalOffsetInfo::prepareWrite()
  noexcept(false)
{
  this->checkConnection();

  int next_id=0;
  if(getId()==0){
    next_id=fetchNextId();
  }

  try {
    m_writeStmt = m_conn->createStatement();
    m_writeStmt->setSQL("INSERT INTO "+getTable()+" ( rec_id, tag, version) " 
			" VALUES ( :1, :2, :3 ) " );

    m_writeStmt->setInt(1, next_id);
    m_ID=next_id;

  } catch (SQLException &e) {
    throw(std::runtime_error("ODFEPedestalOffsetInfo::prepareWrite():  "+e.getMessage()));
  }

}

void ODFEPedestalOffsetInfo::setParameters(const std::map<string,string>& my_keys_map){
  
  // parses the result of the XML parser that is a map of 
  // string string with variable name variable value 
  
  for( std::map<std::string, std::string >::const_iterator ci=
	 my_keys_map.begin(); ci!=my_keys_map.end(); ci++ ) {
    
    if(ci->first==  "VERSION") setVersion(atoi(ci->second.c_str()) );
    if(ci->first==  "TAG") setConfigTag(ci->second);
    
  }
  
}

void ODFEPedestalOffsetInfo::writeDB()
  noexcept(false)
{
  this->checkConnection();
  this->checkPrepare();

  try {

    // number 1 is the id 
    m_writeStmt->setString(2, this->getConfigTag());
    m_writeStmt->setInt(3, this->getVersion());

    m_writeStmt->executeUpdate();


  } catch (SQLException &e) {
    throw(std::runtime_error("ODFEPedestalOffsetInfo::writeDB():  "+e.getMessage()));
  }


  // Now get the ID
  if (!this->fetchID()) {
    throw(std::runtime_error("ODFEPedestalOffsetInfo::writeDB:  Failed to write"));
  } else {
    int old_version=this->getVersion();
    m_readStmt = m_conn->createStatement(); 
    this->fetchData (this);
    m_conn->terminateStatement(m_readStmt);
    if(this->getVersion()!=old_version) std::cout << "ODFEPedestalOffsetInfo>>WARNING version is "<< getVersion()<< endl; 
  }



}


void ODFEPedestalOffsetInfo::fetchData(ODFEPedestalOffsetInfo * result)
  noexcept(false)
{
  this->checkConnection();
  result->clear();
  if(result->getId()==0 && (result->getConfigTag()=="") ){
    throw(std::runtime_error("ODFEPedestalOffsetInfo::fetchData(): no Id defined for this ODFEPedestalOffsetInfo "));
  }

  try {
    if(result->getId()!=0) { 
      m_readStmt->setSQL("SELECT * FROM " + getTable() +   
			 " where  rec_id = :1 ");
      m_readStmt->setInt(1, result->getId());
    } else if (result->getConfigTag()!="") {
      m_readStmt->setSQL("SELECT * FROM " + getTable() +   
			 " where  tag=:1 AND version=:2 " );
      m_readStmt->setString(1, result->getConfigTag());
      m_readStmt->setInt(2, result->getVersion());
    } else {
      // we should never pass here 
      throw(std::runtime_error("ODFEPedestalOffsetInfo::fetchData(): no Id defined for this record "));
    }

    ResultSet* rset = m_readStmt->executeQuery();

    rset->next();

    // 1 is the id and 2 is the config tag and 3 is the version

    result->setId(rset->getInt(1));
    result->setConfigTag(rset->getString(2));
    result->setVersion(rset->getInt(3));

  } catch (SQLException &e) {
    throw(std::runtime_error("ODFEPedestalOffsetInfo::fetchData():  "+e.getMessage()));
  }
}

void ODFEPedestalOffsetInfo::fetchLastData(ODFEPedestalOffsetInfo * result)
  noexcept(false)
{
  this->checkConnection();
  result->clear();
  try {

    m_readStmt->setSQL("SELECT * FROM " + getTable() +   
                       " where  rec_id = ( select max(rec_id) from "+ getTable() +" ) " );
    ResultSet* rset = m_readStmt->executeQuery();

    rset->next();

    result->setId(rset->getInt(1));
    result->setConfigTag(rset->getString(2));
    result->setVersion(rset->getInt(3));

  } catch (SQLException &e) {
    throw(std::runtime_error("ODFEPedestalOffsetInfo::fetchData():  "+e.getMessage()));
  }
}

int ODFEPedestalOffsetInfo::fetchID()    noexcept(false)
{
  // Return from memory if available
  if (m_ID!=0) {
    return m_ID;
  }

  this->checkConnection();

  try {
    Statement* stmt = m_conn->createStatement();
    stmt->setSQL("SELECT rec_id FROM "+ getTable()+
                 "WHERE  tag=:1 and version=:2 " );

    stmt->setString(1, getConfigTag() );
    stmt->setInt(2, getVersion() );

    ResultSet* rset = stmt->executeQuery();

    if (rset->next()) {
      m_ID = rset->getInt(1);
    } else {
      m_ID = 0;
    }
    m_conn->terminateStatement(stmt);
  } catch (SQLException &e) {
    throw(std::runtime_error("ODFEPedestalOffsetInfo::fetchID:  "+e.getMessage()));
  }

  return m_ID;
}
