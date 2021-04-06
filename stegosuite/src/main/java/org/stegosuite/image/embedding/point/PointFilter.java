package org.stegosuite.image.embedding.point;

import org.stegosuite.image.format.ImageFormat;

import java.awt.*;
import java.util.Collection;

/**
 * A filter can be applied to prevent certain points from being generated by the PointGenerator
 */
public abstract class PointFilter<T extends ImageFormat> {

	/**
	 * Contains the list of points that should be skipped when embedding or extracting
	 */
	private Collection<Point> filteredPoints = null;

	/**
	 * Returns the number of LSBs that the filter limits the embedding method to
	 * 
	 * @return
	 */
	public abstract int maxLsbCount();

	/**
	 * Generates a list of points that should NOT be used when embedding or extracting. Implemented
	 * by the subclasses.
	 * 
	 * @param image
	 * @return
	 */
	protected abstract Collection<Point> filter(T image);

	/**
	 * Returns the list of points that should NOT be used when embedding or extracting
	 * 
	 * @return
	 */
	public Collection<Point> getFilteredPoints(T image) {
		if (filteredPoints == null) {
			filteredPoints = filter(image);
		}
		return filteredPoints;
	}

}
