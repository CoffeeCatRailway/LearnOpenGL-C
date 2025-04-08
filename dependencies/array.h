/*
 * Semi-typeless dynamic array for C
 * If you want to define a new array type put 'ARRAY_H_DEFINE_ARRAY(TYPE_HERE)'
 * e.g. ARRAY_H_DEFINE_ARRAY(car_t) // 'car_t' would be a struct
 *
 * Version: 2.1
 * Created by Duncan (CoffeeCatRailway) on 01/04/2025.
 * https://gist.github.com/CoffeeCatRailway/c55f8f56aaf40e2ecd5c3c6994370289
 *
 * TODO:
 *  - Push front (currently only have push back)
 *  - Remove value
 *  - Sort, qsort(array_int->array, array_int->size, sizeof(int), compare); // This will work for now
 *  - Filter & find_at
 *
 * Changelog 1.0:
 *	- Initial code
 *
 * Changelog 1.1:
 *	- Added missing includes
 *
 * Changelog 1.2:
 *	- Changed methods to 'array_type_create' instead of 'arraytypeCreate'
 *
 * 02/04/2025
 * Changelog 1.3:
 *	- Added 'capacityIncrement' (default 2) instead of doubling capacity
 *	- Safer 'realloc' handling
 *
 * Changelog 1.4:
 *	- Added 'ARRAY_NO_INLINE'
 *
 * Changelog 2.0:
 *	- Implemented flexible array member, thank you u\lordlod for telling me about it
 *	  I'd consider this a major change since this changes how memory is allocated
 *
 * 03/04/2025
 * Changelog 2.1:
 *	- Added 'array_type_remove_at', does not reallocate memory use 'array_type_adjust'
 *	- Removed 'ARRAY_NO_INLINE'
 *	- Added 'array_type_adjust', use after removing large amounts from array
 */

#ifndef ARRAY_H_
#define ARRAY_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_H_DEFINE_ARRAY(type) \
	typedef struct array_##type##_t \
	{ \
		size_t size; \
		size_t capacity; \
		size_t capacityIncrement; \
		type* array; \
	} array_##type##_t; \
	static array_##type##_t* array_##type##_create(const size_t capacity) \
	{ \
		array_##type##_t* array = malloc(sizeof(array_##type##_t) + capacity * sizeof(type)); \
		array->size = 0; \
		array->capacity = capacity; \
		array->capacityIncrement = 2; \
		array->array = (type*) (array + 1); \
		return array; \
	} \
	static void array_##type##_delete(array_##type##_t* array) \
	{ \
		array->array = NULL; \
		array->size = array->capacity = 0; \
		free(array); \
	} \
	static void array_##type##_push(array_##type##_t* array, type element) \
	{ \
		array->size++; \
		if (array->size > array->capacity) \
		{ \
			array->capacity += array->capacityIncrement; \
			array_##type##_t* newArray = realloc(array, sizeof(array_##type##_t) + array->capacity * sizeof(type)); \
			if (newArray == NULL) \
			{ \
				printf("Out of memory!\n"); \
				array_##type##_delete(newArray); \
				array_##type##_delete(array); \
				exit(EXIT_FAILURE); \
			} \
			array = newArray; \
			array->array = (type*) (array + 1); \
		} \
		array->array[array->size - 1] = element; \
	} \
	static type array_##type##_remove_at(array_##type##_t* array, const size_t i) \
	{ \
		const type value = array->array[i]; \
		if (i < array->size - 1) \
		{ \
			const size_t segmentSize = (array->size - i - 1) * sizeof(type); \
			memcpy(&array->array[i], &array->array[i + 1], segmentSize); \
		} \
		array->size--; \
		return value; \
	} \
	static void array_##type##_adjust(array_##type##_t* array) \
	{ \
		const size_t capacityAdjusted = array->size - 1 - ((array->size - 1) % array->capacityIncrement) + array->capacityIncrement; \
		if (capacityAdjusted != array->capacity) \
		{ \
			array->capacity = capacityAdjusted; \
			array_##type##_t* newArray = realloc(array, sizeof(array_##type##_t) + array->capacity * sizeof(type)); \
			if (newArray == NULL) \
			{ \
				printf("Out of memory!\n"); \
				array_##type##_delete(newArray); \
				array_##type##_delete(array); \
				exit(EXIT_FAILURE); \
			} \
			array = newArray; \
			array->array = (type*) (array + 1); \
		} \
	}

// ARRAY_H_DEFINE_ARRAY(int) // array_int_t
typedef struct array_float_t
{
	size_t size;
	size_t capacity;
	size_t capacityIncrement;
	float* array;
} array_float_t;

static array_float_t* array_float_create(const size_t capacity)
{
	array_float_t* array = malloc(sizeof(array_float_t) + capacity * sizeof(float));
	array->size = 0;
	array->capacity = capacity;
	array->capacityIncrement = 2;
	array->array = (float*) (array + 1);
	return array;
}

static void array_float_delete(array_float_t* array)
{
	array->array = NULL;
	array->size = array->capacity = 0;
	free(array);
}

static void array_float_push(array_float_t** array, float element)
{
	(*array)->size++;
	if ((*array)->size > (*array)->capacity)
	{
		(*array)->capacity += (*array)->capacityIncrement;
		array_float_t* newArray = realloc(*array, sizeof(array_float_t) + (*array)->capacity * sizeof(float));
		if (newArray == NULL)
		{
			printf("Out of memory!\n");
			array_float_delete(newArray);
			array_float_delete(*array);
			exit(EXIT_FAILURE);
		}
		*array = newArray;
		(*array)->array = (float*) (*array + 1);
	}
	(*array)->array[(*array)->size - 1] = element;
}

static float array_float_remove_at(array_float_t** array, const size_t i)
{
	const float value = (*array)->array[i];
	if (i < (*array)->size - 1)
	{
		const size_t segmentSize = ((*array)->size - i - 1) * sizeof(float);
		memcpy(&(*array)->array[i], &(*array)->array[i + 1], segmentSize);
	}
	(*array)->size--;
	return value;
}

static void array_float_adjust(array_float_t** array)
{
	const size_t capacityAdjusted = (*array)->size - 1 - (((*array)->size - 1) % (*array)->capacityIncrement) + (*array)->
			capacityIncrement;
	if (capacityAdjusted != (*array)->capacity)
	{
		(*array)->capacity = capacityAdjusted;
		array_float_t* newArray = realloc(array, sizeof(array_float_t) + (*array)->capacity * sizeof(float));
		if (newArray == NULL)
		{
			printf("Out of memory!\n");
			array_float_delete(newArray);
			array_float_delete(*array);
			exit(EXIT_FAILURE);
		}
		*array = newArray;
		(*array)->array = (float*) (array + 1);
	}
} // array_float_t
// ARRAY_H_DEFINE_ARRAY(double) // array_double_t
// ARRAY_H_DEFINE_ARRAY(long) // array_long_t
// ARRAY_H_DEFINE_ARRAY(short) // array_short_t
// ARRAY_H_DEFINE_ARRAY(char) // array_char_t

// To use 'unsigned int' you need to define a 'uint' type
// typedef unsigned int uint;
// ARRAY_H_DEFINE_ARRAY(uint) // array_uint_t

// You could also make an array of an array
// Tested by making type 'array_intptr_t'
// typedef array_int_t* array_intptr_t;
// ARRAY_H_DEFINE_ARRAY(array_intptr_t) // array_array_intptr_t_t

// Simply removing elements from an array will not reallocate memory or recalculate capacity
// If you want to do so, call 'array_type_adjust' afterward
// This will reallocate (shrink) memory && lower capacity to nearest increment
// array_int_t* array_int = array_int_create(2);	// size=0 capacity=2
// array_int_push(array_int, 1);					// size=1 capacity=2
// array_int_push(array_int, 2);					// size=2 capacity=2
// array_int_push(array_int, 3);					// size=3 capacity=4
// array_int_remove_at(array_int, 0);				// size=2 capacity=4
// array_int_adjust(array_int);						// size=2 capacity=2

#endif /* ARRAY_H_ */
