#include "stdafx.h"
#include "subregion.h"
#include <algorithm>

 void subregion::init_backlinks(){
	 for (subregion& child : children) {
		 child.parent = this;
		 child.init_backlinks();
	 }
}



 void subregion::calculateCoordinates()
 {
	 //https://en.wikipedia.org/wiki/List_of_centroids
	 //Circular segment
	 //The goal here is to calculate the barycenter of each of our pie slices
	 //In hopes that doing a distance search from hardware-vendor-specified-point to the closest barycenter
	 //will yield the closest region.

	 //Unknown if this is true. It seems to work. 

	 double alpha_rad = to_radians(angular_distance(ang.min, ang.max) / 2.0);

	 //centroid calculation
	 double initial_x = (2.0 * std::sin(alpha_rad)) / (3 * std::abs(alpha_rad));
	 double initial_y = 0;

	 //Since the circular segment may be rotated, we need to fix that. So we see how close it is from 0 and 360 (which are the same - forward)
	 //and then correct by that angle.
	 double dist_from_0 = 0 - angle_between(ang.min, ang.max);
	 double dist_from_360 = 360 - angle_between(ang.min, ang.max);

	 double angular_offset = 0;
	 if (abs(dist_from_360) > abs(dist_from_0)) {
		 angular_offset = to_radians(dist_from_0);
	 }
	 else {
		 angular_offset = to_radians(dist_from_360);
	 }

	 //standard equations to rotate a point about a circle
	 double x_rot = std::cos(angular_offset) * initial_x;
	 double y_rot = std::sin(angular_offset) * initial_x;

	 //center of Z is just the midpoint
	 coords.z = 0.5 * (seg.max + seg.min);

	 coords.x = x_rot;
	 coords.y = y_rot;
 }



 bool subregion::contains(double segment_ratio, double angle_degrees) const {
	 return
		 (seg.min <= segment_ratio && segment_ratio <= seg.max) &&
		 (is_between(angle_degrees, ang.min, ang.max));
 }




 // Find the subregion corresponding to the given named region. Returns nullptr if no matching subregion is found.
 subregion* subregion::find(shared_region some_region) {
	 if (this->region == some_region) {
		 return this;
	 }
	 else {
		 for (subregion& child : children) {
			 subregion* ptr = child.find(some_region);
			 if (ptr != nullptr) {
				 return ptr;
			 }
		 }

		 return nullptr;
	 }
 }

 // Finds the closest named region corresponding to a set of coordinates given by the segment ratio and angle, 
std::pair<double, subregion::shared_region> subregion::find_best_match(double segment_ratio, double angle_degrees) const {

	using DistanceToRegion = std::pair<double, shared_region>;

	 //We're searching for the closest named region. There's two possibilities:
	 //Either this current one is the closest (case 1), or the closest is yet to be found, within the children (case 2).

	 //When we declare the subregion data structure, we must make sure that each parent wholly encloses the 
	 //children regions. We want to be able to say if !parent.contains(x,y), then for each child !child.contains(x,y). 

	 //Note that the inverse is not necessarily true: a parent may contain a point that none of the children contain,
	 //because we don't necessarily name all the regions inside a parent.


	 // Case 1
	 //If there's no children, then there's only one possibility at this level of recursion:
	 //We're the closest.

	 auto my_distance_info = std::make_pair(get_distance(coords, segment_ratio, angle_degrees), region);

	 if (children.empty()) {
		 return my_distance_info;
	 }


	 // Case 2
	 //Well, if there are children - we have the two possibilities on the table: this one's the closest,
	 //or one of the children is, or one of the children's children.. etc.

	 //aka best match = min(this, best_matches(all_children))

	 //Also, what if we had a heuristic that could throw away whole subtrees if 
	 //we knew they couldn't possibly contain the point? We do: .contains(x,y).

	 std::vector<DistanceToRegion> candidates;
	 for (const subregion& child : children) {
		 if (child.contains(segment_ratio, angle_degrees)) {
			 auto best_child_match = child.find_best_match(segment_ratio, angle_degrees);
			 candidates.push_back(best_child_match);
		 }
		 else {
			 //We should still include this child in case it is, in fact, the closest
			 //We just won't search its children, because they can't possibly contain the point.
			 //But now that I think of it, it could be that a child actually is closer than the parent, because
			 //its barycenter is closer. So actually we probably should still fully search each child..
			 candidates.emplace_back(get_distance(child.coords, segment_ratio, angle_degrees), child.region);
		 }
	 }


	 auto least_distance = [](const DistanceToRegion& lhs, const DistanceToRegion& rhs) {
		 return lhs.first < rhs.first;
	 };

	 auto best_of_children = std::min_element(candidates.begin(), candidates.end(), least_distance);
	 return std::min(my_distance_info, *best_of_children, least_distance);
 }