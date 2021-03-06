function [feat_array, feat_tmp,anchor_pt,normal_vec,head_pt,head_vec] = computeStateFeatures(state,LINK_LENGTH,LINK_RADIUS,Tregister,linkStartDraw,voxels,step,maxdist,goal_pt)
%modified the draw state function to compute the normals to all snake links
% function [h, snakePoints] = drawState(state,drawColor,LINK_LENGTH,LINK_RADIUS,drawType,Tregister,linkStartDraw)

% extract information for the transformation matrix
qx = state(1); qy = state(2); qz = state(3);
rz = state(4); ry = state(5); rx = state(6);

% compute the transformation matrix for the first link
T = eye(4); T(1,4) = qx; T(2,4) = qy; T(3,4) = qz;
T(1,1:3) = [cos(ry)*cos(rz), -cos(rx)*sin(rz)+sin(rx)*sin(ry)*cos(rz), sin(rx)*sin(rz)+cos(rx)*sin(ry)*cos(rz)];
T(2,1:3) = [cos(ry)*sin(rz), cos(rx)*cos(rz)+sin(rx)*sin(ry)*sin(rz), -sin(rx)*cos(rz)+cos(rx)*sin(ry)*sin(rz)];
T(3,1:3) = [-sin(ry), sin(rx)*cos(ry), cos(rx)*cos(ry)];

% draw the first link

snakePoints = [];
if (linkStartDraw == 0)
    point1 = Tregister*T*[-LINK_LENGTH; 0; 0; 1];
    point2 = Tregister*T*[0; 0; 0; 1];
    for u = 0:0.1:1.0,
        snakePoints = [snakePoints, [(1-u).*point1 + u.*point2]];
    end
end

% compute the number of extra segments
numAdditionalSegments = (length(state)-6)/2;
normal_vec = cell([numAdditionalSegments,1]);
anchor_pt = cell([numAdditionalSegments,1]);
for j = 1:numAdditionalSegments,
    
    % extract phi and theta
    phi = state(2*j+5);
    theta = state(2*j+6);
    
    
    Tapply(1,:) = [cos(phi), -sin(phi)*cos(theta-pi/2.0), -sin(phi)*sin(theta-pi/2.0), 0];
    Tapply(2,:) = [sin(phi)*cos(theta-pi/2.0), cos(phi)*cos(theta-pi/2.0)*cos(theta-pi/2.0)+sin(theta-pi/2.0)*sin(theta-pi/2.0), cos(phi)*cos(theta-pi/2.0)*sin(theta-pi/2.0)-sin(theta-pi/2.0)*cos(theta-pi/2.0), 0];
    Tapply(3,:) = [sin(phi)*sin(theta-pi/2.0), cos(phi)*sin(theta-pi/2.0)*cos(theta-pi/2.0)-cos(theta-pi/2.0)*sin(theta-pi/2.0), cos(phi)*sin(theta-pi/2.0)*sin(theta-pi/2.0)+cos(theta-pi/2.0)*cos(theta-pi/2.0), 0];
    Tapply(4,:) = [0, 0, 0, 1];
    
    % rotate the transformation matrix according to phi and theta
    T(1:3,1:3) = T(1:3,1:3)*Tapply(1:3,1:3);
    %display(Tapply);
    % calculate the yaw and pitch values from the transformation matrix
    rz = atan2(T(2,1), T(1,1));
    ry = atan2(-T(3,1), sqrt(T(3,2)^2 + T(3,3)^2));
    %display(T);
    % move forward the position by LINK_LENGTH, keep the orientation the same
    T(1,4) = T(1,4) + cos(rz)*cos(ry)*LINK_LENGTH;
    T(2,4) = T(2,4) + sin(rz)*cos(ry)*LINK_LENGTH;
    T(3,4) = T(3,4) - sin(ry)*LINK_LENGTH;
    
    % draw the link
    if (j >= linkStartDraw)
        point1 = Tregister*T*[-LINK_LENGTH; 0; 0; 1];
        point2 = Tregister*T*[0; 0; 0; 1];
        [normal_vec{j},anchor_pt{j},~] = computeLinkNormals(T, LINK_LENGTH, LINK_RADIUS);
        for u = 0:0.1:1.0,
            snakePoints = [snakePoints, [(1-u).*point1 + u.*point2]];
        end
    end
end

snakePoints = snakePoints(1:3,:)';

head_pt = snakePoints(end,:);
head_vec = (snakePoints(end,:)-snakePoints(end-1,:))/norm((snakePoints(end,:)-snakePoints(end-1,:)));

num_links = length(anchor_pt);
num_facades = size(anchor_pt{1},1);
feat_array = zeros([3*num_facades + 6,1]);
% counter = 1;
feat_tmp = zeros([num_links,num_facades]);
for i=1:num_links
    for j=1:num_facades
        feat_tmp(i,j) = distance2obstacle(anchor_pt{i}(j,:),normal_vec{i}(j,:),voxels,step,maxdist);
    end
end
if num_links>=3
    sub_numlinks = [(num_links-mod(num_links,3))/3,(num_links-mod(num_links,3))/3,mod(num_links,3)+(num_links-mod(num_links,3))/3];
    for block=1:3
        for j=1:num_facades
            try
                feat_array((block-1)*num_facades + j) = mean(feat_tmp(sum(sub_numlinks(1:block-1))+1:sum(sub_numlinks(1:block-1)) + sub_numlinks(block),j));
            catch
                keyboard
            end
        end
    end
    
elseif num_links ==1
    feat_array(1:3*num_facades) = repmat(feat_tmp(1,:),[1,3]);
elseif num_links==3
    feat_array(1:num_facades) = feat_tmp(1,:);
    feat_array(num_facades+1:2*num_facades) = feat_tmp(2,:);
    feat_array(2*num_facades+1:3*num_facades) = feat_tmp(2,:);
end
feat_array(3*num_facades + 1) = distance2obstacle(head_pt,head_vec,voxels,step,maxdist);
head_vec = head_vec;

distanceToGoal=norm(snakePoints(end,:)-goal_pt);
feat_array(3*num_facades + 2) = distanceToGoal;
cur_vec=snakePoints(end,:)-snakePoints(end-1,:);
req_vec=goal_pt-snakePoints(end,:);
angleToGoal= acos(cur_vec*req_vec'/(norm(cur_vec)*norm(req_vec)));
feat_array(3*num_facades + 3) = angleToGoal;

% Those points are used to compute bounding box
spx=snakePoints(:,1);spy=snakePoints(:,2);spz=snakePoints(:,3);
xadd=[spx(1);spx(1);spx(1);spx(1);spx(2);spx(2);spx(2);spx(2)];
yadd=[spy(1)+LINK_RADIUS;spy(1)-LINK_RADIUS;spy(1);spy(1);spy(2)+LINK_RADIUS;spy(2)-LINK_RADIUS;spy(2);spy(2)];
zadd=[spz(1);spz(1);spz(1)-LINK_RADIUS;spz(1)+LINK_RADIUS;spz(1);spz(2);spz(2)-LINK_RADIUS;spz(2)+LINK_RADIUS];

[~,~,volume,surface,edgelength] = minboundbox([snakePoints(:,1);xadd],[snakePoints(:,2);yadd],[snakePoints(:,3);zadd]);
feat_array(3*num_facades + 4) = volume;
feat_array(3*num_facades + 5) = surface;
feat_array(3*num_facades + 6) = edgelength;

end
