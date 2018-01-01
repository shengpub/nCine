#include "gtest_array.h"

namespace {

class ArrayOperationsTest : public ::testing::Test
{
  public:
	ArrayOperationsTest() : array_(Capacity) { }

  protected:
	void SetUp() { initArray(array_); }

	nc::Array<int> array_;
};

TEST_F(ArrayOperationsTest, Clear)
{
	ASSERT_FALSE(array_.isEmpty());
	array_.clear();
	printArray(array_);
	ASSERT_TRUE(array_.isEmpty());
	ASSERT_EQ(array_.size(), 0u);
	ASSERT_EQ(array_.capacity(), Capacity);
}

TEST_F(ArrayOperationsTest, CopyConstruction)
{
	printf("Creating a new array with copy construction\n");
	nc::Array<int> newArray(array_);
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayOperationsTest, AssignmentOperator)
{
	printf("Creating a new array with the assignment operator\n");
	nc::Array<int> newArray = array_;
	printArray(newArray);

	ASSERT_EQ(array_.size(), newArray.size());
	for (unsigned int i = 0; i < array_.size(); i++)
		ASSERT_EQ(array_[i], newArray[i]);
}

TEST_F(ArrayOperationsTest, RemoveFirstWithIterator)
{
	printf("Removing the first element with an iterator\n");
	array_.erase(array_.begin());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_[0], 1);
}

TEST_F(ArrayOperationsTest, RemoveLastWithIterator)
{
	printf("Removing the last element with an iterator\n");
	array_.erase(array_.end() - 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity - 1);
	ASSERT_EQ(array_[array_.size() - 1], 8);
}

TEST_F(ArrayOperationsTest, RemoveFirstHalfWithIterator)
{
	printf("Removing the first half of the array\n");
	array_.erase(array_.begin(), array_.begin() + array_.size() / 2);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 5);
	ASSERT_EQ(array_[array_.size() - 1], 9);
}

TEST_F(ArrayOperationsTest, RemoveSecondHalfWithIterator)
{
	printf("Removing the second half of the array\n");
	array_.erase(array_.begin() + array_.size() / 2, array_.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 0);
	ASSERT_EQ(array_[array_.size() - 1], 4);
}

TEST_F(ArrayOperationsTest, RemoveHalfThenShrink)
{
	printf("Removing the second half of the array\n");
	array_.erase(array_.begin() + array_.size() / 2, array_.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_.capacity(), Capacity);
	ASSERT_EQ(array_[0], 0);
	ASSERT_EQ(array_[array_.size() - 1], 4);

	printf("Shrinking the array capacity to match its size\n");
	array_.shrinkToFit();
	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_.capacity(), Capacity / 2);
}

TEST_F(ArrayOperationsTest, RemoveFirstHalfWithReturnedIterator)
{
	nc::Array<int>::Iterator it = array_.begin();

	printf("Removing the first half of the array with the returned iterator\n");
	for (unsigned int i = 0; i < Capacity / 2; i++)
		it = array_.erase(it);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 5);
	ASSERT_EQ(array_[array_.size() - 1], 9);
}

TEST_F(ArrayOperationsTest, RemoveRangeWithReturnedIterator)
{
	nc::Array<int>::Iterator it = array_.begin();

	printf("Removing a range of elements of the array with the returned iterator\n");
	for (unsigned int i = 0; i < Capacity / 2; i++)
		it = array_.erase(it, it + 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity / 2);
	ASSERT_EQ(array_[0], 5);
	ASSERT_EQ(array_[array_.size() - 1], 9);
}

TEST_F(ArrayOperationsTest, RemoveEmptyBeginRange)
{
	printf("Removing an empty begin-to-begin range of the array\n");
	array_.erase(array_.begin(), array_.begin());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(ArrayOperationsTest, RemoveEmptyEndRange)
{
	printf("Removing an empty end-to-end range of the array\n");
	array_.erase(array_.end() - 1, array_.end() - 1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(ArrayOperationsTest, InsertValueAtFrontWithIterator)
{
	printf("Inserting a single value at the beginning of the array\n");
	array_.insert(array_.begin(), -1);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity + 1);
	ASSERT_EQ(array_[0], -1);
	ASSERT_EQ(array_[1], 0);
	ASSERT_EQ(array_[array_.size() - 1], 9);
}

TEST_F(ArrayOperationsTest, InsertAtFrontWithIterator)
{
	nc::Array<int> newArray(Capacity);
	initArray(newArray);

	printf("Inserting the first half of a new array at the beginning of the first one\n");
	array_.insert(array_.begin(), newArray.begin(), newArray.begin() + newArray.size() / 2);
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity + newArray.size() / 2);
	ASSERT_EQ(array_[0], 0);
	ASSERT_EQ(array_[Capacity / 2 - 1], 4);
	ASSERT_EQ(array_[Capacity / 2], 0);
	ASSERT_EQ(array_[array_.size() - 1], 9);
}

TEST_F(ArrayOperationsTest, InsertAtBackWithIterator)
{
	nc::Array<int> newArray(Capacity);
	initArray(newArray);

	printf("Inserting the second half of a new array at the end of the first one\n");
	array_.insert(array_.end(), newArray.begin() + newArray.size() / 2, newArray.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity + newArray.size() / 2);
	ASSERT_EQ(array_[0], 0);
	ASSERT_EQ(array_[Capacity - 1], 9);
	ASSERT_EQ(array_[Capacity], 5);
	ASSERT_EQ(array_[array_.size() - 1], 9);
}

TEST_F(ArrayOperationsTest, InsertWithReturnedIterator)
{
	nc::Array<int> newArray(Capacity);
	nc::Array<int>::Iterator it = newArray.begin();

	printf("Inserting elements in a new array with the returned iterator\n");
	for (unsigned int i = 0; i < Capacity; i++)
		it = newArray.insert(it, static_cast<int>(i));
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(newArray[i], static_cast<int>(i));
}

TEST_F(ArrayOperationsTest, InsertRangeWithReturnedIterator)
{
	nc::Array<int> newArray(Capacity);
	nc::Array<int>::Iterator it = newArray.begin();

	printf("Inserting a range of elements in a new array with the returned iterator\n");
	for (unsigned int i = 0; i < Capacity; i += 2)
		it = newArray.insert(it, array_.begin() + i, array_.begin() + i + 2);
	printArray(newArray);

	ASSERT_EQ(newArray.size(), Capacity);
	for (unsigned int i = 0; i < newArray.size(); i++)
		ASSERT_EQ(newArray[i], static_cast<int>(i));
}

TEST_F(ArrayOperationsTest, InsertEmptyBeginRangeAtFrontWithIterator)
{
	nc::Array<int> newArray(Capacity);
	initArray(newArray);

	printf("Inserting an empty begin-to-begin range at the beginning of the array\n");
	array_.insert(array_.begin(), newArray.begin(), newArray.begin());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

TEST_F(ArrayOperationsTest, InsertEmptyEndRangeAtBackWithIterator)
{
	nc::Array<int> newArray(Capacity);
	initArray(newArray);

	printf("Inserting an empty end-to-end range at the end of the array\n");
	array_.insert(array_.end(), newArray.end(), newArray.end());
	printArray(array_);

	ASSERT_EQ(array_.size(), Capacity);
	ASSERT_TRUE(isUnmodified(array_));
}

}