#pragma once
#include "SharedTypes.h"
#include <vector>
#include "subregion_utils.h"


///
/// Subregions, when combined, create a hierarchical definition of the human body. 
/// For each independently moving body part of the body, we define a subregion which contains the entire part.
/// For example, lower_arm, upper_arm, lower_leg, etc. These subregions can then contain child regions which more specifically
/// divide up the region. Each child subregion must be wholly contained in the parent, but it's not necessary for all the children to 
/// completely cover the parent region. This flexibility allows us to specify useful areas, and if a hardware dev asks for something not within
/// those areas, the core will select the next best thing.
struct subregion {
	using shared_region = NullSpace::SharedMemory::nsvr_shared_region;

	// The named region that we have assigned to this subregion
	shared_region region;

	segment_range seg;
	angle_range ang;

	cartesian_barycenter coords;

	// Any child subregions are listed here
	std::vector<subregion> children;

	// Back pointer to parent, necessary to traverse the hierarchy upwards
	subregion* parent;

	std::vector<std::string> hardware_defined_regions;


	subregion();

	subregion(shared_region region, segment_range segment_offset, angle_range angle_range);

	subregion(shared_region region, segment_range segment_offset, angle_range angle_range, std::vector<subregion> child_regions);

	// Patches up the hierarchy, linking each child to the parent. This should only be called after the entire hierarchy is setup
	void init_backlinks();


	void calculateCoordinates();



	bool contains(double segment_ratio, double angle_degrees) const;

	

	// Find the subregion corresponding to the given named region. Returns nullptr if no matching subregion is found.
	subregion* find(shared_region some_region);

	// Finds the closest named region corresponding to a set of coordinates given by the segment ratio and angle, 
	std::pair<double, shared_region> find_best_match(double segment_ratio, double angle_degrees) const;



};