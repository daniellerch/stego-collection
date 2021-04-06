package org.stegosuite.util;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Array;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;
import java.util.Iterator;
import java.util.Random;
import java.util.function.Predicate;

public class ByteUtils {

	/**
	 * Private constructor to hide the publicly implicit one
	 */
	private ByteUtils() {}

	/**
	 * Generates a byte[] from a given integer
	 *
	 * @param i the integer
	 * @return the integer representation in byte[]
	 */
	public static byte[] intToBytes(int i) {
		return ByteBuffer.allocate(4).putInt(i).array();
	}

	/**
	 * Creates a new array containing the concatenation of all passed arrays
	 *
	 * @param arrays
	 * @return
	 */
	public static byte[] concat(byte[]... arrays) {
		if (arrays.length == 0) {
			return new byte[0];
		}

		if (arrays.length == 1) {
			return arrays[0];
		}

		int length = Arrays.stream(arrays).mapToInt(Array::getLength).sum();
		ByteBuffer concatenation = ByteBuffer.allocate(length);
		Arrays.stream(arrays).forEach(concatenation::put);
		return concatenation.array();
	}

	/**
	 * Copies all content from the InputStream to the OutputStream
	 *
	 * @param in
	 * @param out
	 * @throws IOException
	 */
	public static void copy(InputStream in, OutputStream out)
			throws IOException {
		byte[] buffer = new byte[1024];
		int len;
		while ((len = in.read(buffer)) > 0) {
			out.write(buffer, 0, len);
		}
	}

	/**
	 * Returns one randomly generated byte based on a predicate
	 *
	 * @param predicate
	 */
	public static byte getRandomByte(Predicate<Byte> predicate) {
		Random random = new Random();
		do {
			byte[] b = new byte[1];
			random.nextBytes(b);
			if ((predicate == null) || predicate.test(b[0])) {
				return b[0];
			}
		} while (true);
	}

	/**
	 * Converts an array of 8 bits to one byte. If you pass LITTLE_ENDIAN as byteOrder, index 0 is
	 * LSB, index 7 is MSB. If you pass BIG_ENDIAN as byteOrder, index 0 is MSB, index 7 is LSB.
	 *
	 * @param bits
	 * @param byteOrder
	 * @return
	 */
	public static byte bitsToByte(byte[] bits, ByteOrder byteOrder) {
		byte b = 0;
		for (int i = 0; i < 8; i++) {
			int index = byteOrder.equals(ByteOrder.LITTLE_ENDIAN) ? i : 7 - i;
			if (bits[index] != 0) {
				b = (byte) (b | (1 << i));
			}
		}
		return b;
	}

	/**
	 * Returns the bit at the specified index of b. Index 0 is LSB, index 7 is MSB
	 *
	 * @param b byte from which to read the bit
	 * @param index index to retrieve
	 * @return
	 */
	public static byte getBitAt(byte b, int index) {
		return getBitAt((int) b, index);
	}

	/**
	 * Sets the bit in the passed byte at the specified index
	 * 
	 * @param b byte to modify
	 * @param index index to modify
	 * @param bit new bit value to set, either 1 or 0
	 * @return
	 */
	public static byte setBitAt(byte b, int index, int bit) {
		return (byte) setBitAt((int) b, index, bit);
	}

	/**
	 * Returns the bit at the specified index of i. Index 0 is LSB, index 31 is MSB
	 * 
	 * @param i integer from which to read the bit
	 * @param index index to retrieve
	 * @return
	 */
	public static byte getBitAt(int i, int index) {
		return (byte) ((i & (1 << index)) >> index);
	}

	/**
	 * Sets the bit in the passed integer at the specified index
	 * 
	 * @param i integer to modify
	 * @param index index to modify
	 * @param bit new bit value to set, either 1 or 0
	 * @return
	 */
	public static int setBitAt(int i, int index, int bit) {
		int mask = 1 << index;
		return bit == 1 ? i | mask : i & ~mask;
	}

	/**
	 * Returns true if LSB is 0, otherwise false
	 * 
	 * @param i
	 * @return
	 */
	public static boolean isEven(int i) {
		return i % 2 == 0;
	}

	/**
	 * Returns true if LSB is 1, otherwise false
	 * 
	 * @param i
	 * @return
	 */
	public static boolean isOdd(int i) {
		return i % 2 != 0;
	}

	/**
	 * Returns an Iterable that yields bits for all bytes provided in the input byte array, going
	 * from LSB to MSB for each byte
	 *
	 * @param bytes
	 * @param byteOrder
	 * @return
	 */
	public static Iterable<Byte> iterateBits(byte[] bytes, ByteOrder byteOrder) {
		return () -> new Iterator<Byte>() {

			private int byteIndex = 0;
			private int bitIndex = 0;

			@Override
			public boolean hasNext() {
				return !((byteIndex == bytes.length) && (bitIndex == 0));
			}

			@Override
			public Byte next() {
				byte orderedBitIndex = (byte) (byteOrder == ByteOrder.LITTLE_ENDIAN ? bitIndex : 7 - bitIndex);
				byte b = getBitAt(bytes[byteIndex], orderedBitIndex);
				if ((bitIndex = ++bitIndex % 8) == 0) {
					byteIndex++;
				}
				return b;
			}
		};
	}

}