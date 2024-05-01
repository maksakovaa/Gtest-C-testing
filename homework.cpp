#include <gtest/gtest.h>
#include <gmock/gmock.h>

class DBConnectionInterface
{
public:
    DBConnectionInterface() {}
    virtual ~DBConnectionInterface() {}

    virtual bool open() = 0;
    virtual bool close() = 0;
    virtual std::string execQuery(const std::string& query) = 0;
};

class DBConnection : public DBConnectionInterface
{
public:
    DBConnection() {}
    virtual ~DBConnection() {}

    virtual bool open() override
    {
        status = true;
        return true;
    }
    virtual bool close() override
    {
        if (status)
        {
            status = false;
            return true;
        }
        else
        {
            return false;
        }
    }
    virtual std::string execQuery(const std::string& query) override
    {
        if (status)
        {
            return (query + " success");
        }
        else
        {
            return (query + " error!");
        }
    }
private:
    bool status;
};

class MockDBConnection: public DBConnectionInterface
{
public:
    MOCK_METHOD(bool, open, (), (override));
    MOCK_METHOD(bool, close, (), (override));
    MOCK_METHOD(std::string, execQuery, (const std::string& query), (override));
};

class ClassThatUsesDB
{
public:
    ClassThatUsesDB() {}
    ~ClassThatUsesDB() {}
    bool openConnection(DBConnectionInterface& connector)
    {
        return connector.open();
    }
    std::string useConnection(DBConnectionInterface& connector, std::string query)
    {
        return connector.execQuery(query);
    }
    bool closeConnection(DBConnectionInterface& connector)
    {
        return connector.close();
    }
};

class TestSuite : public ::testing::Test
{
protected:
    void SetUp()
    {
        dbUser = new ClassThatUsesDB();
        mDBConnection = new MockDBConnection();
    }

    void TearDown()
    {
        delete dbUser;
        delete mDBConnection;
    }

protected:
    ClassThatUsesDB* dbUser;
    MockDBConnection* mDBConnection;
};

TEST_F(TestSuite, open)
{
    EXPECT_CALL(*mDBConnection, open).WillOnce(::testing::Return(true));
    dbUser->openConnection(*mDBConnection);
}

TEST_F(TestSuite, close)
{
    EXPECT_CALL(*mDBConnection, close).WillOnce(::testing::Return(false));
    dbUser->closeConnection(*mDBConnection);
}

class TestSuite2 : public ::testing::Test
{
protected:
    void SetUp()
    {
        dbUser = new ClassThatUsesDB();
        dbCon = new DBConnection();
    }

    void TearDown()
    {
        delete dbUser;
        delete dbCon;
    }

protected:
    ClassThatUsesDB* dbUser;
    DBConnection* dbCon;
};

TEST_F(TestSuite2, execQuery1)
{
    std::string expectedResult = "query1 success";
    dbUser->openConnection(*dbCon);
    std::string result = dbUser->useConnection(*dbCon, "query1");

    ASSERT_EQ(result, expectedResult);
}

TEST_F(TestSuite2, execQuery2)
{
    std::string expectedResult = "query1 error!";
    std::string result = dbUser->useConnection(*dbCon, "query1");

    ASSERT_EQ(result, expectedResult);
}

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}