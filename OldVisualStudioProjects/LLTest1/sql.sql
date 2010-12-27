CREATE TABLE node ( node_id integer primary key );

CREATE TABLE edge ( start_node_id integer references node(node_id), 
	end_node_id integer references node(node_id));


node_id = orginial_id
FOR node_id SELECT end_node_id FROM edge WHERE start_node_id = node_id
	depth = depth + 1;
	IF depth > 1 AND node_id = original_id
		RAISE NOTICE 'Inserted loop';
END;


CREATE TRIGGER prevent_loops BEFORE INSERT OR UPDATE N edge FOR EACH ROW EXECUTE PROCEDURE prevent_loops_func ( );



CREATE TABLE reached( id int primary key )
INSERT INTO reached VALUES ( root_id )
WHILE num_rows > 0
  INSERT INTO reached (id ) SELECT DISTINCT end_node_id
  FROM edges e JOIN reached p ON p.id = e.start_node_id
  WHERE e.end_node_id NOT IN ( SELECT id FROM reached )
END