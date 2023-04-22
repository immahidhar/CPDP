package edu.fsu.omp.api;

import edu.fsu.omp.data.ProductDTO;
import edu.fsu.omp.entity.Product;
import edu.fsu.omp.repo.ProductRepository;
import edu.fsu.omp.util.ModelMapperUtil;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import javax.transaction.Transactional;
import java.util.Collections;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.Callable;
import java.util.stream.Collectors;
import java.util.stream.StreamSupport;

@Slf4j
@Controller
@RequestMapping(path="/product")
public class ProductController {
    @Autowired
    private ModelMapperUtil modelMapper;
    @Autowired
    private ProductRepository productRepository;
    @GetMapping(path="/get_all")
    @ResponseStatus(HttpStatus.OK)
    public @ResponseBody List<ProductDTO> getAllProducts() {
        return StreamSupport.stream(productRepository.findAll().spliterator(), false)
                .map(this::convertToDTO).collect(Collectors.toList());
    }
    @GetMapping(path="/get")
    @ResponseStatus(HttpStatus.OK)
    public @ResponseBody List<ProductDTO> getProduct(@RequestParam(required = false) String id,
                                                     @RequestParam(required = false) String name,
                                                     @RequestParam(required = false) String category) {
        if(id != null && Integer.parseInt(id) != 0)
            return Collections.singletonList(convertToDTO(productRepository.findById(Integer.parseInt(id)).get()));
        else if(name != null)
            return productRepository.findByName(name).stream()
                    .map(this::convertToDTO).collect(Collectors.toList());
        else if(category != null)
            return productRepository.findByCategory(category).stream()
                    .map(this::convertToDTO).collect(Collectors.toList());
        return null;
    }
    @Transactional
    @PostMapping(path="/add")
    public ResponseEntity<String> addNewProduct(@RequestBody ProductDTO productDTO) {
        productRepository.save(modelMapper.map(productDTO, Product.class));
        return ResponseEntity.status(HttpStatus.CREATED).body("Product added");
    }
    @Transactional
    @PutMapping(path="/update")
    public Callable<ResponseEntity<String>> updateProduct(@RequestBody ProductDTO productDTO) {
        return () -> {
            Optional<Product> product = productRepository.findById(productDTO.getId());
            if (!product.isPresent())
                return ResponseEntity.status(HttpStatus.NOT_ACCEPTABLE).body("Product not available!");
            product.get().setName(productDTO.getName());
            product.get().setCategory(productDTO.getCategory().toString());
            product.get().setPrice(productDTO.getPrice());
            product.get().setQuantity(productDTO.getQuantity());
            product.get().setAddress(productDTO.getAddress());
            productRepository.save(product.get());
            return ResponseEntity.status(HttpStatus.ACCEPTED).body("Product updated successfully!");
        };
    }
    @Transactional
    @DeleteMapping(path="/delete")
    public ResponseEntity<String> deleteProduct(@RequestParam(required = false) String id,
                                           @RequestParam(required = false) String name) {
        if(id != null && Integer.parseInt(id)!=0) productRepository.deleteById(Integer.parseInt(id));
        else if(name != null) productRepository.deleteByName(name);
        return ResponseEntity.status(HttpStatus.ACCEPTED).body("Product deleted!");
    }
    @Transactional
    @DeleteMapping(path="/delete_all")
    public ResponseEntity<String> deleteAllProducts() {
        log.info("Deleting all products from database!!!");
        productRepository.deleteAll();
        return ResponseEntity.status(HttpStatus.ACCEPTED).body("All products deleted!!!");
    }
    private ProductDTO convertToDTO(Product product) {return modelMapper.map(product, ProductDTO.class);}
}
