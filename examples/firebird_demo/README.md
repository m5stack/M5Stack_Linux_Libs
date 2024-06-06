# firebird
Firebird is a full-featured, powerful, efficient, lightweight, and maintenance-free database. It makes it easy for you to upgrade from a single user, single database to enterprise-level applications.

A Firebird database server can manage multiple independent databases, each of which can support multiple client connections simultaneously. In short, it is an open-source, powerful, and freely available database (even for commercial use).

Features:
1. Firebird is a true relational database that supports stored procedures, views, triggers, transactions, and all the features of large relational databases.  
2. Firebird supports the majority of SQL92 commands and most SQL99 commands, with the newer version Firebird 2.0 providing even more complete support for SQL99.  
3. Firebird's source code is based on the mature commercial database Interbase, ensuring good stability and compatibility with Interbase.  
4. No need to worry about licensing fees (it's free), and you don't have to fear being sued by the database developer for using pirated software in the future.  
5. Easy to deploy, with installation files just a few megabytes in size, highly customizable, and simple client distribution requiring only a DLL file.  
6. Firebird has an embedded server version that runs directly without installation, making it a preferred choice for single-machine development.  
7. Firebird has very high operational efficiency.  
8. Highly portable, capable of running on Linux, Unix, MacOS, Windows, Solaris systems with the same database format, requiring no modifications.  
9. Excellent development environment support, where applications can be developed based on Firebird directly using native development interfaces without the need for ODBC connections, such as in Delphi and C++Builder.  

This engineering demo allows your CoreMP135 device to transform into a simple IoT database server!

## Installation
The official website of Firebird provides precompiled binary files of the Firebird database, which are available for each platform. You just need to go to [firebird](https://www.firebirdsql.org/) to get the installation package of the corresponding platform.

Installation on ARM:
```bash
sudo apt install libtommath1
wget https://github.com/FirebirdSQL/firebird/releases/download/v5.0.0/Firebird-5.0.0.1306-0-linux-arm32.tar.gz
tar zxvf Firebird-5.0.0.1306-0-linux-arm32.tar.gz
cd Firebird-5.0.0.1306-0-linux-arm32
sudo ./install
```

## Local
You can use /opt/firebird/bin/isql for management locally.  
Create a database:
```bash
cd /opt/firebird/bin
./isql
SQL> CREATE DATABASE '/opt/firebird/test_database.fdb' USER 'test' PASSWORD 'test';
SQL> quit;
```

Connect to the database for CRUD operations:
```bash
cd /opt/firebird/bin
./isql /opt/firebird/test_database.fdb -u test -p test
# View all tables
SQL> SHOW TABLES;
# If the test_table does not exist, create it
SQL> SET TERM !! ; EXECUTE BLOCK AS BEGIN IF (NOT EXISTS (SELECT 1 FROM rdb$relations WHERE rdb$relation_name = 'TEST_TABLE')) THEN BEGIN EXECUTE STATEMENT 'CREATE TABLE test_table (id INTEGER NOT NULL PRIMARY KEY, name VARCHAR(100))'; END END!! SET TERM ; !!
SQL> COMMIT;
# Insert two rows of data
SQL> INSERT INTO test_table (id, name) VALUES (1, 'John Doe');
SQL> INSERT INTO test_table (id, name) VALUES (2, 'Jane Doe');
# Commit the operation
SQL> COMMIT;
# View the data in the database
SQL> SELECT * FROM test_table;
# Modify the values in the database
SQL> UPDATE test_table SET name = 'John Smith' WHERE id = 1;
SQL> COMMIT;
SQL> SELECT * FROM test_table;
# Delete the values in the database
SQL> DELETE FROM test_table WHERE id = 2;
SQL> COMMIT;
SQL> SELECT * FROM test_table;
# Delete the table
# SQL> COMMIT;
# SQL> DROP TABLE test_table;
# SQL> COMMIT;
# exit
SQL> QUIT;

```


## Remote 
An account needs to be created for remote use：
```bash
cd /opt/firebird/bin
./gsec -user sysdba -password masterkey -add newuser -pw newpassword
./gsec -user sysdba -password masterkey -modify existinguser -pw newpassword
./gsec -user sysdba -password masterkey -delete username
./gsec -user sysdba -password masterkey -display
```
- sysdba is the username for the administrator.
- masterkey is the password for the administrator.
- newuser is the username to be added.
- newpassword is the password for the new user.
- existinguser is the user to be modified.
- username is the user to be deleted.

Install dependencies：
```bash
sudo apt install libtommath1
wget https://github.com/FirebirdSQL/firebird/releases/download/v5.0.0/Firebird-5.0.0.1306-0-linux-x64.tar.gz
tar zxvf Firebird-5.0.0.1306-0-linux-x64.tar.gz
cd Firebird-5.0.0.1306-0-linux-x64
tar zxvf buildroot.tar.gz
export LD_LIBRARY_PATH=`pwd`/buildroot/opt/firebird/lib:$LD_LIBRARY_PATH
sudo apt install python3 python3-pip
pip3 install fdb
```
### python
```python
import fdb

con = fdb.connect(
    host='127.0.0.1',
    database='/opt/firebird/test_database.fdb', 
    user='test',
    password='test'
)

cur = con.cursor()

cur.execute('''
SET TERM !! ;
EXECUTE BLOCK AS
BEGIN
  IF (NOT EXISTS (
    SELECT 1 FROM rdb$relations 
    WHERE rdb$relation_name = 'TEST_TABLE'
  )) THEN
  BEGIN
    EXECUTE STATEMENT '
      CREATE TABLE test_table (
        id INTEGER NOT NULL PRIMARY KEY,
        name VARCHAR(100)
      )
    ';
  END
END!!
SET TERM ; !!
''')
con.commit()

cur.execute("INSERT INTO test_table (id, name) VALUES (1, 'John Doe')")
cur.execute("INSERT INTO test_table (id, name) VALUES (2, 'Jane Doe')")
con.commit()

cur.execute("SELECT * FROM test_table")
for row in cur.fetchall():
    print(row)

cur.execute("UPDATE test_table SET name = 'John Smith' WHERE id = 1")
con.commit()

# cur.execute("DELETE FROM test_table WHERE id = 2")
# con.commit()

cur.close()
con.close()
```


## To use ODBC (Open Database Connectivity) on Ubuntu, you need to install the following software and configure certain files:

1. **Install Necessary Software**:
 - **unixODBC**: This is the driver manager for ODBC on Unix systems. You can install it using the following command:
 ```
 sudo apt-get install unixODBC unixODBC-dev 
 ```
 - **ODBC Driver**: Depending on the database you want to connect to, you might need to install the specific ODBC driver. For example, for MySQL, you can install `libmyodbc` or `myodbc-installer`. For Firebird， you need down [Firebird odbc Driver](https://firebirdsql.org/en/odbc-driver/), and create a symbolic link from firebird libfbclient.so.5.0.0 to /lib/libgds.so.


2. **Configure ODBC**:
 - **odbcinst.ini**: This file is used to configure the ODBC drivers. It typically resides in `/etc/odbcinst.ini`. Here is an example configuration for a Firebird driver:
 ```
[Firebird]
Description = Firebird ODBC driver
Driver = /usr/lib/x86_64-linux-gnu/odbc/libOdbcFb.so
Setup = /usr/lib/x86_64-linux-gnu/odbc/libOdbcFb.so
 ```
 - **odbc.ini**: This file is used to configure the data sources. It typically resides in `/etc/odbc.ini`. Here is an example configuration for a data source named `FirebirdDSN`:
  ```
  [FirebirdDSN]
  Description = Firebird Database
  Driver = Firebird
  Dbname = 127.0.0.1:/opt/firebird_db_files/database.fdb
  User = xxxxx
  Password = xxxxx
  ```

3. **Test the Connection**:
 - You can test the ODBC connection using the `isql` command, which is part of the unixODBC package:
 ```
 isql -v FirebirdDSN
 ```
 - Replace `FirebirdDSN` with the name of your data source as configured in `odbc.ini`.

Make sure to replace the paths and configurations with the appropriate values for your system and database setup.






